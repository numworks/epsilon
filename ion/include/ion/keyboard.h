#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

#include <stdbool.h>

typedef enum {
  ION_KEY_1,
  ION_KEY_2,
  ION_KEY_PLUS,
  ION_KEY_MINUS,
  ION_NUMBER_OF_KEYS // Should be defined last
} ion_key_t;

bool ion_scankey(ion_key_t key);

char ion_getchar();

#endif
