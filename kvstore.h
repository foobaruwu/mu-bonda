#ifndef KVSTORE_H
#define KVSTORE_H
#pragma once

#include "lib.h"

#define MAX_ENTRIES 1024
#define HASH_TABLE_SIZE 1024  
#define MEMORY_POOL_SIZE (MAX_ENTRIES * 2) 

typedef struct __attribute__((packed, aligned(8))) {
  uint32_t key : 31;
  uint32_t value : 31;
  uint8_t is_signed : 1;
  uint8_t is_tombstone : 1;
} kv_entry_t;

typedef struct hash_entry {
  uint32_t key;
  int log_index;
  union {
      struct hash_entry* next; 
      int next_free;           
  };
  int is_used;  
} hash_entry_t;

void kv_init(void);
int kv_put(uint32_t key, uint32_t value, uint32_t is_signed);
int kv_get(uint32_t key, uint32_t *value, uint32_t *is_signed);
int kv_delete(uint32_t key);
int kv_print_log(int count);


#endif
