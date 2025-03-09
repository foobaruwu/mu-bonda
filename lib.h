#ifndef LIB_H
#define LIB_H

#pragma once

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define SIGNED_MAX 1073741823
#define SIGNED_MIN -1073741824
#define UNSIGNED_MIN 0
#define UNSIGNED_MAX 2147483647

#define CORRECTED_SIGN(x)                                                      \
  ((int)x > SIGNED_MAX) ? (int)(((int)x - UNSIGNED_MAX) - 1) : (int)x

// K&R Implementation
void itoa(int val, char *buf, int _base);

#endif // !LIB_H
