#include "multicore.h"
#include "uart.h"

volatile core_control_t core_control[MAX_CORES];

static inline void dmb(void) {
    asm volatile("dmb sy" : : : "memory");
}

static inline void dsb(void) {
    asm volatile("dsb sy" : : : "memory");
}

uint32_t get_core_id(void) {
    uint32_t core_id;
    asm volatile("mrs %0, mpidr_el1" : "=r" (core_id));
    return core_id & 3; 
}

void multicore_init(void) {
    for (int i = 0; i < MAX_CORES; i++) {
        core_control[i].status = (i == 0) ? CORE_BUSY : CORE_IDLE;
        core_control[i].task = NULL;
        core_control[i].arg = NULL;
        core_control[i].completed = 1;
    }
    dmb();
}

int core_execute_task(uint32_t core_id, core_task_fn task, void* arg) {
    if (core_id >= MAX_CORES || core_id == get_core_id()) {
        return -1;
    }
    
    while (core_control[core_id].status != CORE_IDLE) {
        // yield the data here or small delay if needed
    }
    
    core_control[core_id].completed = 0;
    core_control[core_id].task = task;
    core_control[core_id].arg = arg;
    dmb();  
    
    core_control[core_id].status = CORE_BUSY;
    dsb();  
    
    return 0;
}

uint64_t core_wait_for_completion(uint32_t core_id) {
    if (core_id >= MAX_CORES) {
        return -1;
    }
    
    while (core_control[core_id].completed == 0) {
        // yield or small delay if needed
    }
    
    return core_control[core_id].result;
}

void secondary_main(void) {
    uint32_t core_id = get_core_id();
    
    core_control[core_id].status = CORE_IDLE;
    dmb();
    
    while (1) {
        if (core_control[core_id].status == CORE_BUSY && core_control[core_id].task != NULL) {
            core_task_fn task = core_control[core_id].task;
            void* arg = core_control[core_id].arg;
            
            task(arg);
            
            core_control[core_id].task = NULL;
            core_control[core_id].completed = 1;
            dmb();
            core_control[core_id].status = CORE_IDLE;
        }
    }
}

int execute_on_any_core(core_task_fn task, void* arg) {
    for (int i = 1; i < MAX_CORES; i++) {  
        if (core_control[i].status == CORE_IDLE) {
            return core_execute_task(i, task, void* arg);
        }
    }
    return -1; 
}

void wait_for_all_cores(void) {
    for (int i = 1; i < MAX_CORES; i++) {
        if (core_control[i].status == CORE_BUSY) {
            core_wait_for_completion(i);
        }
    }
}