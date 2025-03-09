#ifndef UART_H
#define UART_H

// god bless these osdevs on discord pitust(696410219663851552
// ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIGSvTSRFfSQC5HFWsgdWN734
// QAotzxd+oUpjCE9fpqdv key-v2@pitust.dev) and clever___(204112
// 578425782272)
#define UART_BASE 0x1c00030000
#define UART_REG(x) (*((volatile unsigned long int *)(x)))

#define UART0_DR (UART_BASE + 0x00)
#define UART0_FR (UART_BASE + 0x18)
#define UART0_IBRD (UART_BASE + 0x24)
#define UART0_FBRD (UART_BASE + 0x28)
#define UART0_LCR_H (UART_BASE + 0x2C)
#define UART0_CR (UART_BASE + 0x30)
#define UART0_ICR (UART_BASE + 0x44)

#define FR_TXFF (1 << 5)
#define FR_RXFE (1 << 4)




// None of these are needed too

// #define LCR_H_FEN (1 << 4)
// #define LCR_H_WLEN_8 (3 << 5)
// #define CR_UARTEN (1 << 0)
// #define CR_TXE (1 << 8)
// #define CR_RXE (1 << 9)

// void uart_init();
//
// NOTE: we dont really need to write our own uart_init
// provided we have the following config.txt
//
// ```
// enable_rp1_uart=1
// pciex4_reset=0
// ```
//
// enable_rp1_uart does all the heavy lifting for us
// and we should see `RP1_UART 0000001c00030000`
// thrown out of uart during boot
// Although we should be writing our own uart handler, for other benefits, should be good for now

// TODO: error handling for uart functions
// Core functions
void uart_putc(char c);
char uart_getc();

// Added functionality
void uart_puts(const char *s);
void uart_gets(char *buf, int size);
void uart_ascii(unsigned int value, char *buf);
void uart_ascii_signed(int value, char *buf);

#endif
