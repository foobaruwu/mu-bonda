#include "uart.h"

void uart_putc(char c) {
  while (UART_REG(UART0_FR) & FR_TXFF)
    ;
  UART_REG(UART0_DR) = c;
}

char uart_getc() {
  while (UART_REG(UART0_FR) & FR_RXFE)
    ;
  return UART_REG(UART0_DR);
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
