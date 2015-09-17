#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

#include <stdbool.h>

typedef enum {
  ION_KEY_A_1,
  ION_KEY_A_2,
  ION_KEY_A_3,
  ION_KEY_A_4,
  ION_KEY_A_5,

  ION_KEY_B_1,
  ION_KEY_B_2,
  ION_KEY_B_3,
  ION_KEY_B_4,
  ION_KEY_B_5,

  ION_KEY_FAKE_0,
  ION_KEY_FAKE_1,
  ION_KEY_C_1,
  ION_KEY_C_2,
  ION_KEY_C_3,

  ION_KEY_D_1,
  ION_KEY_D_2,
  ION_KEY_D_3,
  ION_KEY_D_4,
  ION_KEY_D_5,

  ION_KEY_E_1,
  ION_KEY_E_2,
  ION_KEY_E_3,
  ION_KEY_E_4,
  ION_KEY_E_5,

  ION_KEY_F_1,
  ION_KEY_F_2,
  ION_KEY_F_3,
  ION_KEY_F_4,
  ION_KEY_F_5,

  ION_KEY_G_1,
  ION_KEY_G_2,
  ION_KEY_G_3,
  ION_KEY_G_4,
  ION_KEY_G_5,

  ION_KEY_LAST
} ion_key_t;

#define ION_NUMBER_OF_KEYS 35

bool ion_key_state(ion_key_t key);

// This is our keymap
char ion_getchar();

#endif
