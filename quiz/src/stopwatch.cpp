#include <ion/timing.h>
#include <quiz/stopwatch.h>
#include <string.h>

#include "quiz.h"

uint64_t quiz_stopwatch_start() { return Ion::Timing::millis(); }

static size_t uint64ToString(uint64_t n, char buffer[]) {
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

void quiz_stopwatch_print_lap(uint64_t startTime) {
  constexpr char Time[] = " time: ";
  constexpr char Ms[] = "ms";
  constexpr size_t uint64ToStringMaxLength = 20;
  constexpr size_t MaxLength =
      sizeof(Time) + uint64ToStringMaxLength + sizeof(Ms) + 1;
  char buffer[MaxLength];
  char* position = buffer;
  position += strlcpy(position, Time, sizeof(Time));
  position += uint64ToString(Ion::Timing::millis() - startTime, position);
  position += strlcpy(position, Ms, sizeof(Ms));
  quiz_print(buffer);
}
