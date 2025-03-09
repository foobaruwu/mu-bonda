#ifndef KVSTORE_H
#define KVSTORE_H

#define MAX_ENTRIES 1024

typedef struct {
  unsigned long long data;

} kv_entry_t;

void kv_init();

int kv_put(unsigned int key, unsigned int value, unsigned int is_signed);
int kv_get(unsigned int key, unsigned int *value, unsigned int *is_signed);
int kv_delete(unsigned int key);
int kv_print_log();

#endif
