#include "kvstore.h"
#include "uart.h"

static kv_entry_t kv_log[MAX_ENTRIES];
static int kv_index = 0;

void kv_init() { kv_index = 0; }

void kv_put(unsigned int key, unsigned int value, unsigned int is_signed) {
  if (kv_index >= MAX_ENTRIES) {
    uart_puts("KVSTORE FULL\n");
    return;
  }

  unsigned long long entry = ((unsigned long long)key & 0xFFFFFFFF) |
                             (((unsigned long long)value & 0xFFFFFFFF) << 31) |
                             (((unsigned long long)is_signed & 0x1) << 63);

  kv_log[kv_index++] = (kv_entry_t){entry};
  uart_puts("KVSTORE PUT\n");
}

int kv_get(unsigned int key, unsigned int *value, unsigned int *is_signed) {
  for (int i = kv_index - 1; i >= 0; i--) {
    unsigned long long entry = kv_log[i].data;
    if ((entry & 0x7FFFFFFF) == key) {
      if (entry & (1ULL << 62)) {
        return -1;
      }
      *value = (entry >> 31) & 0x7FFFFFFF;
      *is_signed = (entry >> 63) & 0x1;
      uart_puts("KVSTORE GET\n");
      return 0;
    }
  }
  return -1;
}

void kv_delete(unsigned int key) {
  if (kv_index >= MAX_ENTRIES) {
    uart_puts("KVSTORE FULL\n");
  }
  unsigned long long entry =
      ((unsigned long long)key & 0xFFFFFFFF) | (1ULL << 62);
  kv_log[kv_index++] = (kv_entry_t){entry};
  uart_puts("KVSTORE DELETE\n");
}
