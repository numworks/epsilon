
#include <ion/backlight.h>
#include <ion/display.h>
#include <ion/timing.h>

void ion_main(int argc, const char * const argv[]) {
  // Initialize the backlight
  Ion::Backlight::init();
  while (1) {
    Ion::Display::pushRectUniform(KDRect(0,0,10,10), KDColorRed);
    Ion::Timing::msleep(100);
    Ion::Display::pushRectUniform(KDRect(0,0,10,10), KDColorBlue);
    Ion::Timing::msleep(100);
  }
}
