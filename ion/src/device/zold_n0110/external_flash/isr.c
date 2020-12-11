#include "isr.h"

/* Interrupt Service Routines are void->void functions */
typedef void (*ISR)(void);

/* TODO */

#define INITIALISATION_VECTOR_SIZE 1

ISR InitialisationVector[INITIALISATION_VECTOR_SIZE]
  __attribute__((section(".isr_vector_table")))
  __attribute__((used))
  = {
  start, // Reset service routine,
};
