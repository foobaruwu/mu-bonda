#include "kvstore.h"
#include "uart.h"

static kv_entry_t kv_log[MAX_ENTRIES];
static int kv_index = 0;

void kv_init() { kv_index = 0; }

int kv_put(unsigned int key, unsigned int value, unsigned int is_signed) {
  if (kv_index >= MAX_ENTRIES) {
    uart_puts("KVSTORE FULL\n");
    return -1;
  }

  unsigned long long entry = ((unsigned long long)key & 0xFFFFFFFF) |
                             (((unsigned long long)value & 0xFFFFFFFF) << 31) |
                             (((unsigned long long)is_signed & 0x1) << 63);

  kv_log[kv_index++] = (kv_entry_t){entry};
  uart_puts("KVSTORE PUT\n");
  return 0;
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

int kv_delete(unsigned int key) {
  if (kv_index == 0) {
    uart_puts("KVSTORE EMPTY\n");
    return -1;
  }
  unsigned long long entry =
      ((unsigned long long)key & 0xFFFFFFFF) | (1ULL << 62);
  kv_log[kv_index++] = (kv_entry_t){entry};
  uart_puts("KVSTORE DELETE\n");
  return 0;
}

int kv_print_log() {
  char buf[32];
  
  if (kv_index == 0) {
      uart_puts("KVSTORE EMPTY\r\n");
      return -1;
  }
  
  uart_puts("KVSTORE LOG:\r\n");
  uart_puts("------------\r\n");
  
  for (int i = 0; i < kv_index; i++) {
      unsigned long long entry = kv_log[i].data;
      unsigned int key = entry & 0x7FFFFFFF;
      unsigned int value = (entry >> 31) & 0x7FFFFFFF;
      unsigned int is_signed = (entry >> 63) & 0x1;
      unsigned int is_deleted = (entry >> 62) & 0x1;
      
      uart_puts("Entry [");
      uart_ascii(i, buf);
      uart_puts(buf);
      uart_puts("]: ");
      
      uart_puts("Key=");
      uart_ascii(key, buf);
      uart_puts(buf);
      
      if (is_deleted) {
          uart_puts(" (DELETED)\r\n");
      } else {
          uart_puts(", Value=");
          if (is_signed) {
              uart_puts("-");
              int complement = ~value | 1;
              uart_ascii_signed(complement, buf);
              uart_puts(buf);
          } else {
              uart_ascii(value, buf);
              uart_puts(buf);
          }
          uart_puts("\r\n");
      }
  }
  
  uart_puts("------------\r\n");
  uart_puts("Total entries: ");
  uart_ascii(kv_index, buf);
  uart_puts(buf);
  uart_puts("\r\n");
  
  return 0;
}