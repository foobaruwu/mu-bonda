#include "kvstore.h"
#include "lib.h"
#include "uart.h"

static kv_entry_t kv_log[MAX_ENTRIES];
static int kv_index = 0;

void kv_init() { kv_index = 0; }

int kv_put(uint32_t key, uint32_t value, uint32_t is_signed) {
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

int kv_get(uint32_t key, uint32_t *value, uint32_t *is_signed) {
  char buf[64];
  uart_puts("\r\nchecking for ");
  itoa(key, buf, 10);
  uart_puts(buf);
  uart_puts("\r\n");

  for (int i = kv_index - 1; i >= 0; i--) {
    kv_entry_t entry = kv_log[i];

    itoa(entry.key, buf, 10);
    uart_puts("\r\nchecking ");
    uart_puts(buf);
    uart_puts(" with ");
    itoa(key, buf, 10);
    uart_puts(buf);
    uart_puts("\r\n");

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

int kv_delete(uint32_t key) {
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

    uart_puts("Key=");
    itoa(key, buf, 10);
    uart_puts(buf);

    if (is_deleted) {
      uart_puts(" (DELETED)\r\n");
    } else {
      uart_puts(", Value=");
      if (is_signed) {
        uart_puts("-");
        itoa(-value, buf, 10);
        uart_puts(buf);
      } else {
        itoa(value, buf, 0);
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
