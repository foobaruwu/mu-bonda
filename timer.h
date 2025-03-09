#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define RP1_TIMER_BASE 0x7c004000

#define TIMER_CS        0x00
#define TIMER_CLO       0x04
#define TIMER_CHI       0x08
#define TIMER_C0        0x0C
#define TIMER_C1        0x10
#define TIMER_C2        0x14
#define TIMER_C3        0x18

#define TIMER_CS_M0     (1 << 0)
#define TIMER_CS_M1     (1 << 1)
#define TIMER_CS_M2     (1 << 2)
#define TIMER_CS_M3     (1 << 3)

typedef struct {
    uint64_t start_time;
    uint64_t end_time;
} timer_context_t;

void timer_init(void);
uint64_t timer_get_ticks(void);

void timer_delay_us(uint32_t us);
void timer_delay_ms(uint32_t ms);

void timer_start(timer_context_t* timer);
void timer_end(timer_context_t* timer);
uint64_t timer_elapsed_us(timer_context_t* timer);  
uint32_t timer_elapsed_ms(timer_context_t* timer);  

#endif 