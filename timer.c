#include "timer.h"

static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)(reg) = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)(reg);
}


void timer_init(void)
{
    mmio_write(RP1_TIMER_BASE + TIMER_CS, 0xF);
}


uint64_t timer_get_ticks(void)
{
    uint32_t hi, lo;
    
    hi = mmio_read(RP1_TIMER_BASE + TIMER_CHI);
    lo = mmio_read(RP1_TIMER_BASE + TIMER_CLO);
    
    if (hi != mmio_read(RP1_TIMER_BASE + TIMER_CHI)) {
        hi = mmio_read(RP1_TIMER_BASE + TIMER_CHI);
        lo = mmio_read(RP1_TIMER_BASE + TIMER_CLO);
    }
    
    return ((uint64_t)hi << 32) | lo;
}


void timer_delay_us(uint32_t us)
{
    uint64_t start = timer_get_ticks();
    
    uint64_t target = start + us;
    
    while (timer_get_ticks() < target) {
n
        __asm__ volatile("yield");
    }
}


void timer_delay_ms(uint32_t ms)
{
    timer_delay_us(ms * 1000);
}


void timer_start(timer_context_t* timer)
{
    if (timer) {
        timer->start_time = timer_get_ticks();
        timer->end_time = 0;
    }
}


void timer_end(timer_context_t* timer)
{
    if (timer) {
        timer->end_time = timer_get_ticks();
    }
}


uint64_t timer_elapsed_us(timer_context_t* timer)
{
    if (!timer || timer->end_time < timer->start_time) {
        return 0;
    }
    
    return timer->end_time - timer->start_time;
}


uint32_t timer_elapsed_ms(timer_context_t* timer)
{
    return (uint32_t)(timer_elapsed_us(timer) / 1000);
}