#include <ion.h>
#include "display.h"
#include "events_keyboard.h"
#include <emscripten.h>
#include <stdlib.h>

extern "C" {
const char * IonSoftwareVersion();
const char * IonPatchLevel();
}

const char * IonSoftwareVersion() { return Ion::softwareVersion(); }
const char * IonPatchLevel() { return Ion::patchLevel(); }

int main(int argc, char * argv[]) {
  Ion::Display::Emscripten::init();
  Ion::Events::Emscripten::init();

  // Set the seed for random using the current time
  srand(emscripten_get_now());

  ion_main(argc, argv);
  return 0;
}

void Ion::Timing::msleep(uint32_t ms) {
  emscripten_sleep(ms);
}

void Ion::Timing::usleep(uint32_t us) {
  emscripten_sleep(us/1000);
}
