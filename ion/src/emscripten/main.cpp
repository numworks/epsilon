#include <ion.h>
#include "display.h"
#include "events.h"

int main(int argc, char * argv[]) {
  Ion::Display::Emscripten::init();
  Ion::Events::Emscripten::init();
  ion_app();
  return 0;
}

void Ion::msleep(long ms) {
}
