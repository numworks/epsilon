#include <ion.h>
#include "display.h"
#include "events_keyboard.h"
#include "../../../apps/global_preferences.h"

int main(int argc, char * argv[]) {
  Ion::Display::Emscripten::init();
  Ion::Events::Emscripten::init();

  ion_main(argc, argv);
  return 0;
}

void Ion::msleep(long ms) {
}
