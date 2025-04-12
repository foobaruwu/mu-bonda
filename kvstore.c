#include "kvstore.h"
#include "lib.h"
#include "uart.h"
#include "multicore.h"

//TODO:minimize uart puts

static kv_entry_t kv_log[MAX_ENTRIES];
static int kv_index = 0;

static hash_entry_t* hash_table[HASH_TABLE_SIZE];
static hash_entry_t memory_pool[MEMORY_POOL_SIZE];

static int free_list_head = -1;
static int next_free_entry = 0;

typedef struct {
    volatile int lock;
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

static spinlock_t hash_lock;

void spinlock_init(spinlock_t *lock) {
    lock->lock = 0;
}

void spinlock_acquire(spinlock_t *lock) {
    while (__atomic_test_and_set(&lock->lock, __ATOMIC_ACQUIRE)) {
        // Wait
    }
}

void spinlock_release(spinlock_t *lock) {
    __atomic_clear(&lock->lock, __ATOMIC_RELEASE);
}

static uint32_t hash_function(uint32_t key) {
  //hmm modulo seems to be very expensive
  //welp ez hash func because idk a better and nicer one, guess u can use a better one
  key = ((key >> 16) ^ key) * 0x45d9f3b;
  key = ((key >> 16) ^ key) * 0x45d9f3b;
  key = (key >> 16) ^ key;
  return key & (HASH_TABLE_SIZE - 1); 
  //this might be cheaper because bit shifting 
}

static hash_entry_t* memory_pool_alloc(void) {
    int index;
    
    if (free_list_head != -1) {
        index = free_list_head;
        free_list_head = memory_pool[index].next_free;
        memory_pool[index].is_used = 1;
    } else if (next_free_entry < MEMORY_POOL_SIZE) {
        index = next_free_entry++;
        memory_pool[index].is_used = 1;
    } else {
        return 0;
    }
    
    return &memory_pool[index];
}

static void memory_pool_free(hash_entry_t* entry) {
    if (!entry) return;
    
    int index = entry - memory_pool;
    
    if (index < 0 || index >= MEMORY_POOL_SIZE) return;
    
    memory_pool[index].is_used = 0;
    
    memory_pool[index].next_free = free_list_head;
    free_list_head = index;
}

void kv_init(void) { 
    kv_index = 0; 
    spinlock_init(&hash_lock);
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_table[i] = 0;
    }
    
    free_list_head = -1;
    next_free_entry = 0;
    
}

int kv_put(uint32_t key, uint32_t value, uint32_t is_signed) {
    if (kv_index >= MAX_ENTRIES) {
        return -1;
    }

    int new_index = kv_index++;
    kv_log[new_index] = (kv_entry_t){
        .is_signed = is_signed,
        .is_tombstone = 0,
        .key = key,
        .value = value,
    };

    uint32_t hash = hash_function(key);
    
    spinlock_acquire(&hash_lock);
    hash_entry_t* prev = 0;
    hash_entry_t* current = hash_table[hash];
    
    while (current) {
        if (current->key == key) {
            current->log_index = new_index;
            spinlock_release(&hash_lock);
            return 0;
        }
        prev = current;
        current = current->next;
    }
    
    hash_entry_t* new_entry = memory_pool_alloc();
    if (!new_entry) {
        spinlock_release(&hash_lock);
        return -1;
        // this could lead to issues where we run out of space, so maybe compaction can be the remedy
    }
    
    new_entry->key = key;
    new_entry->log_index = new_index;
    new_entry->next = hash_table[hash];
    hash_table[hash] = new_entry;
    spinlock_release(&hash_lock);

    return 0;
}

int kv_get(uint32_t key, uint32_t *value, uint32_t *is_signed) {
    // hash table implemented because who dosent like O(1)
    //Ill add comments later
    uint32_t hash = hash_function(key);
    spinlock_acquire(&hash_lock);
    hash_entry_t* entry = hash_table[hash];
    
    while (entry) {
        if (entry->key == key) {
            kv_entry_t log_entry = kv_log[entry->log_index];
            if (log_entry.is_tombstone) {
                spinlock_release(&hash_lock);
                return -1;  
            }
                
            *value = log_entry.value;
            *is_signed = log_entry.is_signed;
            spinlock_release(&hash_lock);
            return 0;
        }
        entry = entry->next;
    }
    
    spinlock_release(&hash_lock);
    return -1; 
}

int kv_delete(uint32_t key) {
    if (kv_index >= MAX_ENTRIES) {
        return -1;
    }
    
    uint32_t hash = hash_function(key);
    spinlock_acquire(&hash_lock);
    hash_entry_t* entry = hash_table[hash];
    hash_entry_t* prev = 0;
    
    int found = 0;
    while (entry) {
        if (entry->key == key) {
            found = 1;
            
            if (prev) {
                prev->next = entry->next;
            } else {
                hash_table[hash] = entry->next;
            }
            
            memory_pool_free(entry);
            break;
        }
        prev = entry;
        entry = entry->next;
    }
    
    if (!found) {
        spinlock_release(&hash_lock);
        return -1;  
    }
    
    kv_log[kv_index++] = (kv_entry_t){
        .is_signed = 0,
        .is_tombstone = 1,
        .key = key,
        .value = 0,
    };
    spinlock_release(&hash_lock);

    return 0;
}

int kv_print_log(int count) {
    char buf[64];

    if (kv_index == 0) {
        uart_puts("\r\nKVSTORE EMPTY\r\n");
        return -1;
    }

    uart_puts("\r\nKVSTORE LOG:\r\n");
    uart_puts("\r\n------------\r\n");

    // Calculate start and end indices for display
    int start = 0;

    if (count > 0 && count < kv_index) {
        start = kv_index - count;
    }

    // Iterate in reverse order to show newest entries first
    for (int i = kv_index - 1; i >= start; i--) {
        kv_entry_t entry = kv_log[i];
        uint32_t key = entry.key;
        uint32_t value = entry.value;
        uint32_t is_signed = entry.is_signed;
        uint32_t is_deleted = entry.is_tombstone;

        uart_puts("Entry [");
        itoa(i, buf, 10);
        uart_puts(buf);
        uart_puts("]: ");

        uart_puts("Key: ");
        itoa(key, buf, 10);
        uart_puts(buf);

        if (is_deleted) {
            uart_puts(" (DELETED)\r\n");
        } else {
            uart_puts(", Value: ");
            if (is_signed) {
                itoa(-value, buf, 10);
                uart_puts(buf);
            } else {
                itoa(value, buf, 10);
                uart_puts(buf);
            }
            uart_puts("\r\n");
        }
    }

    uart_puts("------------\r\n");
    int displayed = (count > 0 && count < kv_index) ? count : kv_index;
    uart_puts("Displaying ");

    itoa(displayed, buf, 10);
    uart_puts(buf);
    uart_puts(" of ");

    itoa(kv_index, buf, 10);
    uart_puts(buf);
    uart_puts(" total entries\r\n");

    return 0;
}

typedef struct {
    uint32_t key_start;
    uint32_t key_end;
    uint32_t result_count;
} parallel_scan_args_t;

void parallel_scan_task(void* arg) {
    parallel_scan_args_t* scan_args = (parallel_scan_args_t*)arg;
    uint32_t count = 0;
    
    // Scan a portion of the hash table
    for (uint32_t i = scan_args->key_start; i < scan_args->key_end; i++) {
        hash_entry_t* entry = hash_table[i];
        while (entry) {
            if (kv_log[entry->log_index].is_tombstone == 0) {
                count++;
            }
            entry = entry->next;
        }
    }
    
    scan_args->result_count = count;
}

uint32_t kv_count_parallel(void) {
    uint32_t total = 0;
    parallel_scan_args_t args[MAX_CORES-1];
    uint32_t chunk_size = HASH_TABLE_SIZE / MAX_CORES;
    
    for (int i = 1; i < MAX_CORES; i++) {
        args[i-1].key_start = (i-1) * chunk_size;
        args[i-1].key_end = (i == MAX_CORES-1) ? HASH_TABLE_SIZE : i * chunk_size;
        args[i-1].result_count = 0;
        core_execute_task(i, parallel_scan_task, &args[i-1]);
    }
    
    for (uint32_t i = (MAX_CORES-1) * chunk_size; i < HASH_TABLE_SIZE; i++) {
        hash_entry_t* entry = hash_table[i];
        while (entry) {
            if (kv_log[entry->log_index].is_tombstone == 0) {
                total++;
            }
            entry = entry->next;
        }
    }
    
    for (int i = 1; i < MAX_CORES; i++) {
        core_wait_for_completion(i);
        total += args[i-1].result_count;
    }
    
    return total;
}
