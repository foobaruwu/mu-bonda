#include "uart.h"

void uart_init() {
  // disable uart0 before config
  UART0_CR = 0x0;
  // IBRD AND FBRD VALUES FOR 115200
  UART0_IBRD = 26;
  UART0_FBRD = 3;
  // 4th bit is set to 1 for FIFO, 5:6 bits are set to 11 for 8 bit word length
  UART0_LCRH = (1 << 4) | (3 << 5);
  // enable uart0, tx and rx
  UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
  // wait until tx buffer is empty
  while (UART0_FR & (1 << 5))
    ;

  UART0_DR = c;
}

char uart_getc() {
  while (UART0_FR & (1 << 4))
    ;
  return UART0_DR;
}

void uart_puts(const char *s) {
  while (*s) {
    uart_putc(*s++);
  }
}

void uart_gets(char *buf, int size) {
  int i = 0;
  char c;
  while (i <= size - 1) {
    c = uart_getc();
    if (c == '\n' || c == '\r' || c == '\0') {
      break;
    }
    buf[i++] = c;
    uart_putc(c);
  }
  buf[i] = '\0';
}

void uart_ascii(unsigned int value, char *buf) {
  int i = 0;
  char c;
  c = value % 10;
  while (i < 31) {
    buf[31 - i] = c + '0';
    i++;
    value = value / 10;
  }
}

void uart_ascii_signed(int value, char *buf) {
  int i = 0;
  char c;
  c = value % 10;
  while (i < 31) {
    buf[31 - i] = c + '0';
    i++;
    value = value / 10;
  }
}
