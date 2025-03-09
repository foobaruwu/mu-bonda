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

  kv_log[kv_index++] = (kv_entry_t){
      .is_signed = is_signed,
      .is_tombstone = 0,
      .key = key,
      .value = value,
  };

  uart_puts("\r\n[debug]: KVSTORE PUT\r\n");
  return 0;
}

int kv_get(unsigned int key, unsigned int *value, unsigned int *is_signed) {
  for (int i = kv_index - 1; i >= 0; i--) {
    kv_entry_t entry = kv_log[i];
    if (entry.key == key) {
      if (entry.is_tombstone)
        return -1;
      *value = entry.value;
      *is_signed = entry.is_signed;
      uart_puts("\r\nKVSTORE GET\r\n");
    }
  }
  return -1;
}

int kv_delete(unsigned int key) {
  if (kv_index == 0) {
    uart_puts("KVSTORE EMPTY\n");
    return -1;
  };
  // unsigned long long entry =
  //     ((unsigned long long)key & 0xFFFFFFFF) | (1ULL << 62);
  kv_log[kv_index++] = (kv_entry_t){
      .is_signed = 0,
      .is_tombstone = 1,
      .key = key,
      .value = 0,
  };
  uart_puts("\r\nKVSTORE DELETE\r\n");
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
  int end = kv_index;

  if (count > 0 && count < kv_index) {
    start = kv_index - count;
  }

  // Iterate in reverse order to show newest entries first
  for (int i = kv_index - 1; i >= start; i--) {
    kv_entry_t entry = kv_log[i];
    unsigned int key = entry.key;
    unsigned int value = entry.value;
    unsigned int is_signed = entry.is_signed;
    unsigned int is_deleted = entry.is_tombstone;

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
  int displayed = (count > 0 && count < kv_index) ? count : kv_index;
  uart_puts("Displaying ");
  uart_ascii(displayed, buf);
  uart_puts(buf);
  uart_puts(" of ");
  uart_ascii(kv_index, buf);
  uart_puts(buf);
  uart_puts(" total entries\r\n");

  return 0;
}
