#include <ion.h>
#include <string.h>

char charForKey[ION_NUMBER_OF_KEYS] = {
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  'V',
  'W',
  'X',
  'Y',
  'Z',
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8'
};

char ion_getchar() {
  while (1) {
    ion_sleep();
    for (ion_key_t k=0;k<ION_NUMBER_OF_KEYS; k++) {
      if (ion_key_state(k)) {
        return charForKey[k];
      }
    }
  }
  /*
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
*/
}
