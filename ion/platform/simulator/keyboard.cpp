extern "C" {
#include <ion.h>
}
#include "platform.h"

bool ion_scankey(ion_key_t key) {
  return Platform.keyboard->scankey(key);
}

char ion_getchar() {
  for (int i=0; i<100; i++) {
    ion_sleep();
  }
  //ion_sleep();
  //char c = getchar();
  //printf("Returning %c\n", c);
  return '6';
}
