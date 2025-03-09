#ifndef KVSTORE_H
#define KVSTORE_H

#define MAX_ENTRIES 1024

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef struct __attribute__((packed, aligned(8))) {
  uint8_t is_signed : 1;
  uint8_t is_tombstone : 1;
  uint32_t key : 31;
  uint32_t value : 31;
} kv_entry_t;

void kv_init();

int kv_put(unsigned int key, unsigned int value, unsigned int is_signed);
int kv_get(unsigned int key, unsigned int *value, unsigned int *is_signed);
int kv_delete(unsigned int key);
int kv_print_log(int count);

#endif
