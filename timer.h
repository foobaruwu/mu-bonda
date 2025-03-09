#ifndef TIMER_H
#define TIMER_H

#include "lib.h"

#define CLOCK_BASE_OFFSET 0x1c00018000
#define CLOCKS_MAIN_BASE (0x40018000 + CLOCK_BASE_OFFSET)

#define TIMER_COUNT_LO (CLOCKS_MAIN_BASE + 0x0C)  
#define TIMER_COUNT_HI (CLOCKS_MAIN_BASE + 0x10)  

uint64_t timer_get_time_us(void);
void timer_start_command(void);
uint64_t timer_end_command(void);
void timer_print_elapsed(uint64_t microseconds);

#endif 