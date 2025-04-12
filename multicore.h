#ifndef MULTICORE_H
#define MULTICORE_H

#include "lib.h"

#define MAX_CORES 4
#define CORE_READY 1
#define CORE_BUSY 2
#define CORE_IDLE 3

typedef void (*core_task_fn)(void*);

typedef struct {
    volatile uint32_t status;
    volatile core_task_fn task;
    volatile void* arg;
    volatile uint32_t completed;
    volatile uint64_t result;
} core_control_t;

extern volatile core_control_t core_control[MAX_CORES];

uint32_t get_core_id(void);

void multicore_init(void);

int core_execute_task(uint32_t core_id, core_task_fn task, void* arg);

uint64_t core_wait_for_completion(uint32_t core_id);

void secondary_main(void);

int execute_on_any_core(core_task_fn task, void* arg);

void wait_for_all_cores(void);

#endif