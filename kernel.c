#include "kvstore.h"
#include "lib.h"
#include "uart.h"
#include "timer.h" 

int parse_key(const char *str, unsigned int *key);
int parse_input(const char *str, unsigned int *key, unsigned int *value,
                unsigned int *is_signed);

void kernel_main() {
  kv_init();
  uart_puts("KVSTORE INIT\r\n");

  char buf[128];

  while (1) {
    uart_puts("\r\n>");
    uart_gets(buf, 128);
    uart_puts("\r\n");
    
    timer_init();
    timer_context_t my_timer;
    timer_start(&my_timer);

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
      if (parse_key(buf + 4, &key)) {
        int result = kv_get((int)key, &value, &is_signed);
        if (result == 0) {
          uart_puts("\r\nVALUE: ");
          if (is_signed) {
            uart_puts("-");
            itoa(-value, buf, 10);
            uart_puts(buf);
          } else {
            itoa(value, buf, 10);
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
      if (parse_key(buf + 4, &key)) {
        int result = kv_delete(key);
        if (result == 0) {
          uart_puts("\r\nKEY DELETED\r\n");
        } else {
          uart_puts("\r\nFAILED TO DELETE KEY\r\n");
        }
      } else {
        uart_puts("\r\nINVALID INPUT\r\n");
      }
    } else if (buf[0] == 'l' && buf[1] == 'o' && buf[2] == 'g') {
      int count = 10;
      char *param = buf + 3;

      while (*param == ' ')
        param++;

      if (*param == '*') {
        count = -1;
      } else if (*param >= '0' && *param <= '9') {
        count = 0;
        while (*param >= '0' && *param <= '9') {
          count = count * 10 + (*param - '0');
          param++;
        }
      }

      int result = kv_print_log(count);
      if (result == -1) {
        uart_puts("\r\nNO ENTRIES TO DISPLAY\r\n");
      }
    }
    
    timer_end(&my_timer);
    uint64_t elapsed_us = timer_elapsed_us(&my_timer);
    uint32_t elapsed_ms = timer_elapsed_ms(&my_timer);
    uart_puts("\r\nElapsed time: ");
    itoa(elapsed_us, buf, 10);
    uart_puts(buf);
    uart_puts(" us\r\n");
    itoa(elapsed_ms, buf, 10);
    uart_puts(buf);
    uart_puts(" ms\r\n");


  }
}

int parse_key(const char *str, unsigned int *key) {
  *key = 0;
  while (*str >= '0' && *str <= '9') {
    *key = *key * 10 + (*str - '0');
    str++;
  }
  // TEST:
  // char buf[128];
  // int temp = *key;
  // itoa(temp, buf, 10);
  // uart_puts("\r\nparsed key: ");
  // uart_puts(buf);
  // uart_puts("\r\n");
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
