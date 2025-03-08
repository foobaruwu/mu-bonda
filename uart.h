#ifndef UART_H
#define UART_H

#define UART0_BASE 0x1f00030000
#define UART0_DR                                                               \
  (*(volatile unsigned int *)(UART0_BASE + 0x00)) // data
                                                  // register
#define UART0_FR                                                               \
  (*(volatile unsigned int *)(UART0_BASE + 0x18)) // flag
                                                  // register
#define UART0_IBRD                                                             \
  (*(volatile unsigned int *)(UART0_BASE + 0x24)) // integer baud rate register
#define UART0_FBRD                                                             \
  (*(volatile unsigned int *)(UART0_BASE +                                     \
                              0x28)) // fractional baud rate register
#define UART0_LCRH                                                             \
  (*(volatile unsigned int *)(UART0_BASE + 0x2C)) // line control register
#define UART0_CR                                                               \
  (*(volatile unsigned int *)(UART0_BASE + 0x30)) // control register

void uart_init();
void uart_putc(char c);
char uart_getc();
void uart_puts(const char *s);
void uart_gets(char *buf, int size);
void uart_ascii(unsigned int value, char *buf);
void uart_ascii_signed(int value, char *buf);

#endif
