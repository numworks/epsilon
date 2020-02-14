#ifndef QUIZ_STOPWATCH_H
#define QUIZ_STOPWATCH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t quiz_stopwatch_start();
void quiz_stopwatch_print_lap(uint64_t startTime);

#ifdef __cplusplus
}
#endif

#endif
