#include "isr.h"

extern const void * _stack_start;
typedef void (*ISR)(void);

ISR InitialisationVector[2] __attribute__((section(".isr_vector_table"), used)) = {
  (ISR)&_stack_start, // Stack start
  start // Reset service routine,
};
