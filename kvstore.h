#ifndef KVSTORE_H
#define KVSTORE_H
#pragma once

#include "lib.h"

#define MAX_ENTRIES 1024

typedef struct __attribute__((packed, aligned(8))) {
  uint8_t is_signed : 1;
  uint8_t is_tombstone : 1;
  uint32_t key : 31;
  uint32_t value : 31;
} kv_entry_t;

void kv_init();

int kv_put(uint32_t key, uint32_t value, uint32_t is_signed);
int kv_get(uint32_t key, uint32_t *value, uint32_t *is_signed);
int kv_delete(uint32_t key);
int kv_print_log(int count);

#endif
