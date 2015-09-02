#include <ion.h>
#include <string.h>

bool * ion_key_states;

char charForKey[ION_NUMBER_OF_KEYS] = {
  '1',
  '2',
  '3',
  '/',
  '^',
  '.'
};

char ion_getchar() {
  bool key_states[ION_NUMBER_OF_KEYS];
  memcpy(key_states, ion_key_states, ION_NUMBER_OF_KEYS);
  while (1) {
    for (int k=0; k<ION_NUMBER_OF_KEYS; k++) {
      if (ion_key_states[k]) {
        if (!key_states[k]) {
          return charForKey[k];
        }
      } else {
        // Key k is down. So if we ever see it up, we'll want to return a char.
        key_states[k] = 0;
      }
    }
    ion_sleep();
  }
}
