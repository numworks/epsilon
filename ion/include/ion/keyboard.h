#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

#include <stdbool.h>

/* Platforms should simply set the ion_key_states pointer.
 * Clients of Ion, (including ion_getchar) will then dereference this pointer to
 * read the state of the keys: ion_key_states[ion_key] gives the last known
 * state of ion_key */

typedef enum {
  ION_KEY_1,
  ION_KEY_2,
  ION_KEY_PLUS,
  ION_KEY_MINUS,
} ion_key_t;

#define ION_NUMBER_OF_KEYS 4

extern bool * ion_key_states;

char ion_getchar();

#endif
