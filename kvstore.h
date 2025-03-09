#ifndef KVSTORE_H
#define KVSTORE_H

#define MAX_ENTRIES 1024

#define BITMASK(high, low) (((unsigned long long)1 << (high - low + 1)) - 1) << low)

#define KEY_MASK(src) (unsigned int)((src & BITMASK(30, 0) >> 0)
#define VAL_MASK(src) (unsigned int)((src & BITMASK(61, 31) >> 31)
#define TOMBSTONE_MASK(src) (unsigned int)((src & BITMASK(62, 62) >> 62)
#define SIGNED_MASK(src) (unsigned int)((src & BITMASK(63, 63) >> 63)

#define CREATE_ENTRY_ULL(key, value, is_signed, is_tombstone)                  \
  (unsigned long long)((unsigned long long)0 | (unsigned long long)key << 0 |  \
                       (unsigned long long)value << 31 |                       \
                       (unsigned long long)is_tombstone << 62 |                \
                       (unsigned long long)is_signed << 63)

#define CREATE_ENTRY_T(entry) (kv_entry_t){.data = entry};

typedef struct {
  unsigned long long data;
} kv_entry_t;

void kv_init();

int kv_put(unsigned int key, unsigned int value, unsigned int is_signed);
int kv_get(unsigned int key, unsigned int *value, unsigned int *is_signed);
int kv_delete(unsigned int key);
int kv_print_log(int count);

#endif
