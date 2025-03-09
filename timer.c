#include "lib.h"
#include "timer.h"

static uint64_t start_time = 0;
static uint64_t timer_freq = 0;

void timer_init(void) {
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r" (freq));
    timer_freq = freq;
}

uint64_t timer_get_counter(void) {
    uint64_t count;
    asm volatile("mrs %0, cntpct_el0" : "=r" (count));
    return count;
}

void time_start(void) {
    start_time = timer_get_counter();
}

uint64_t time_end(void) {
    uint64_t end_time = timer_get_counter();
    uint64_t ticks = end_time - start_time;
    

    return (ticks * 1000000000) / timer_freq;
}

uint64_t time_end_ticks(void) {
    uint64_t end_time = timer_get_counter();
    return end_time - start_time;
}

uint64_t timer_get_frequency(void) {
    return timer_freq;
}