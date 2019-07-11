#include "ion/include/ion/timing.h"
#include <quiz.h>
#include <stddef.h>

size_t uint64ToString(uint64_t n, char buffer[]) {
  size_t len = 0;
  do {
    buffer[len++] = (n % 10) + '0';
  } while ((n /= 10) > 0);
  int i = 0;
  int j = len - 1;
  while (i < j) {
    char c = buffer[i];
    buffer[i++] = buffer[j];
    buffer[j--] = c;
  }
  return len;
}

void printElapsedTime(uint64_t startTime) {
  char buffer[7+26+2] = " time: ";
  size_t len = uint64ToString((Ion::Timing::millis() - startTime)/1000, buffer+7);
  buffer[7+len] = 's';
  buffer[7+len+1] = 0;
  quiz_print(buffer);
}
