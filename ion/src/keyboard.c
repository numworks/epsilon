#include <ion.h>
#include <string.h>

char charForKey[ION_NUMBER_OF_KEYS] = {
  'A', 'B', 'C', 'D', 'E',
  'F', 'G', 'H', 'I', 'J',
  'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T',
  'U', 'V', 'W', 'X', 'Y',
  'Z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8'
};

char ion_getchar() {
  // Let's start by saving which keys we've seen up
  bool key_seen_up[ION_NUMBER_OF_KEYS];
  for (ion_key_t k=0; k<ION_NUMBER_OF_KEYS; k++) {
    key_seen_up[k] = !ion_key_down(k);
  }

  // Wait a little to debounce the button.
  // FIXME: REAL SLEEP
  for (int i=0;i<10000;i++) {
  }
  /* Let's discard the keys we previously saw up but which aren't anymore: those
   * were probably bouncing! */
  for (ion_key_t k=0; k<ION_NUMBER_OF_KEYS; k++) {
    key_seen_up[k] &= !ion_key_down(k);
  }

  while (1) {
    for (ion_key_t k=0; k<ION_NUMBER_OF_KEYS; k++) {
      if (ion_key_down(k)) {
        if (key_seen_up[k]) {
          return charForKey[k];
        }
      } else {
        key_seen_up[k] = 1;
      }
    }
    ion_sleep();
    // FIXME: REAL SLEEP
    for (int i=0;i<10000;i++) {
    }
  }
}
