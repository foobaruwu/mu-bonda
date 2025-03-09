#include "timer.h"
#include "uart.h"

static uint64_t command_start_time = 0;

uint64_t timer_get_time_us(void) {
    uint32_t hi = *((volatile uint32_t*)TIMER_COUNT_HI);
    uint32_t lo = *((volatile uint32_t*)TIMER_COUNT_LO);
    
    if (hi != *((volatile uint32_t*)TIMER_COUNT_HI)) {
        hi = *((volatile uint32_t*)TIMER_COUNT_HI);
        lo = *((volatile uint32_t*)TIMER_COUNT_LO);
    }
    
    return (((uint64_t)hi << 32) | lo) / 48;
}

void timer_start_command(void) {
    command_start_time = timer_get_time_us();
}

uint64_t timer_end_command(void) {
    uint64_t end_time = timer_get_time_us();
    return end_time - command_start_time;
}

void timer_print_elapsed(uint64_t microseconds) {
    char buf[32];
    
    uart_puts("\r\n[Execution time: ");
    
    if (microseconds < 1000) {
        itoa((int)microseconds, buf, 10);
        uart_puts(buf);
        uart_puts(" μs");
    } else if (microseconds < 1000000) {
        int ms = microseconds / 1000;
        int decimal = (microseconds % 1000) / 10;
        
        itoa(ms, buf, 10);
        uart_puts(buf);
        uart_puts(".");
        
        if (decimal < 10) {
            uart_puts("0");
        }
        if (decimal < 100 && decimal >= 0) {
            uart_puts("0");
        }
        
        itoa(decimal, buf, 10);
        uart_puts(buf);
        uart_puts(" ms");
    } else {
        int sec = microseconds / 1000000;
        int decimal = (microseconds % 1000000) / 1000;
        
        itoa(sec, buf, 10);
        uart_puts(buf);
        uart_puts(".");
        
        if (decimal < 10) {
            uart_puts("00");
        } else if (decimal < 100) {
            uart_puts("0");
        }
        
        itoa(decimal, buf, 10);
        uart_puts(buf);
        uart_puts(" s");
    }
    
    uart_puts("]\r\n");
}
