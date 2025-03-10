#define UART_REG(x) (*((volatile unsigned long int *)(x)))

#define UART_BASE 0x1c00030000

#define UART0_DR (UART_BASE + 0x00)
#define UART0_FR (UART_BASE + 0x18)
#define UART0_IBRD (UART_BASE + 0x24)
#define UART0_FBRD (UART_BASE + 0x28)
#define UART0_LCR_H (UART_BASE + 0x2C)
#define UART0_CR (UART_BASE + 0x30)
#define UART0_ICR (UART_BASE + 0x44)

#define FR_TXFF (1 << 5)
#define FR_RXFE (1 << 4)
#define LCR_H_FEN (1 << 4)
#define LCR_H_WLEN_8 (3 << 5)
#define CR_UARTEN (1 << 0)
#define CR_TXE (1 << 8)
#define CR_RXE (1 << 9)

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

void kernel_main() {
  char c;
  uart_putc('m');
  uart_putc('u');
  uart_putc('b');
  uart_putc('o');
  uart_putc('n');
  uart_putc('d');
  uart_putc('a');
  uart_putc('\n');

  while (1) {
    c = uart_getc();
    uart_putc(c);
  }
}
