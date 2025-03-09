#ifndef _TIMER_H_
#define _TIMER_H_

#include "lib.h"

void timer_init(void);

uint64_t timer_get_counter(void);

void time_start(void);

uint64_t time_end(void);

uint64_t time_end_ticks(void);

uint64_t timer_get_frequency(void);

#endif 