#include "lib.h"

void swap(char *a, char *b) {
  if (!a || !b)
    return;
  // NOTE: can we do XOR ops and
  // remove the third var
  *(a) = *(a) ^ *(b);
  *(b) = *(a) ^ *(b);
  *(a) = *(a) ^ *(b);
}

void reverse(char *buf, int length) {
  int start = 0;
  int end = length - 1;
  while (start < end) {
    swap(&buf[start++], &buf[end--]);
  }
}

// K&R Implementation
// NOTE: need to make this versatile for and
// base
void itoa(int val, char *buf, int _base) {
  int i = 0;
  int sign = 0;

  if ((sign = val) < 0) {
    val = -val;
  }
  do {
    buf[i++] = val % 10 + '0';
  } while ((val /= 10) > 0);
  if (sign < 0)
    buf[i++] = '-';
  buf[i] = '\0';
  reverse(buf, i);
}
