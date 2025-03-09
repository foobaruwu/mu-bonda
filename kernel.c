#include "kvstore.h"
#include "uart.h"

int parse_key(const char *str, unsigned int *key);
int parse_input(const char *str, unsigned int *key, unsigned int *value,
                unsigned int *is_signed);
void kernel_main() {
  // uart_init();
  kv_init();
  uart_puts("KVSTORE INIT\r\n");

  char buf[128];

  while (1) {
    uart_puts("\r\n>");
    uart_gets(buf, 128);

    // uart_puts("\r\nrecv: ");
    // uart_puts(buf);
    // uart_puts("\r\n");

    if (buf[0] == 'p' && buf[1] == 'u' && buf[2] == 't') {
      unsigned int key, value, is_signed = 0;
      if (parse_input(buf + 3, &key, &value, &is_signed)) {
        int result = kv_put(key, value, is_signed);
        if (result == 0) {
          uart_puts("\r\nKEY SET SUCCESSFULLY\r\n");
        } else {
          uart_puts("\r\nFAILED TO SET KEY\r\n");
        }
      } else {
        uart_puts("\r\nINVALID INPUT\r\n");
      }
    } else if (buf[0] == 'g' && buf[1] == 'e' && buf[2] == 't') {
      unsigned int key, value, is_signed = 0;
      if (parse_key(buf + 3, &key)) {
        int result = kv_get(key, &value, &is_signed);
        if (result == 0) {
          uart_puts("\r\nVALUE: ");
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
        } else {
          uart_puts("\r\nKEY NOT FOUND\r\n");
        }
      } else {
        uart_puts("\r\nINVALID INPUT\r\n");
      }
    } else if (buf[0] == 'd' && buf[1] == 'e' && buf[2] == 'l') {
      unsigned int key;
      if (parse_key(buf + 3, &key)) {
        int result = kv_delete(key);
        if (result == 0) {
          uart_puts("\r\nKEY DELETED\r\n");
        } else {
          uart_puts("\r\nFAILED TO DELETE KEY\r\n");
        }
      } else {
        uart_puts("\r\nINVALID INPUT\r\n");
      }
    }
  }
}

int parse_key(const char *str, unsigned int *key) {
  *key = 0;
  while (*str >= '0' && *str <= '9') {
    *key = *key * 10 + (*str - '0');
    str++;
  }
  return (*str == '\0' || *str == ' ');
}

int parse_input(const char *str, unsigned int *key, unsigned int *value,
                unsigned int *is_signed) {
  *key = *value = *is_signed = 0;
  while (*str == ' ')
    str++;
  if (!parse_key(str, key))
    return 0;

  while (*str >= '0' && *str <= '9')
    str++;
  while (*str == ' ')
    str++;

  if (!parse_key(str, value))
    return 0;

  while (*str >= '0' && *str <= '9')
    str++;
  while (*str == ' ')
    str++;

  if (*str == '1') {
    *is_signed = 1;
    str++;
  } else if (*str == '0') {
    *is_signed = 0;
    str++;
  } else {
    return 0;
  }
  return (*str == '\0');
}
