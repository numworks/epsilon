#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

#include <stdbool.h>


typedef enum {
  ION_KEY_1,
  ION_KEY_2,
  ION_KEY_PLUS,
  ION_KEY_MINUS,
} ion_key_t;

#define ION_NUMBER_OF_KEYS 4

extern bool * ion_key_states;

// ion_key_states[ion_key] gives the state of ion_key

char ion_getchar();

#endif
