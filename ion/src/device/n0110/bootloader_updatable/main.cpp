#include <ion.h>

void ColorScreen(uint32_t color) {
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(color));
  Ion::Timing::msleep(300);
}

void ion_main(int argc, const char * const argv[]) {
  // TODO LEA Initialize display with message ?
  Ion::Backlight::init();
  ColorScreen(0x00FF00);
  while (true) {
    Ion::Timing::msleep(3000);
  }
  // Step 1. MPU configuration
  // Step 2. Privileged / Unprivileged
  // Step 3. Light LED blue + other message?
  // Step 4. Jump to third-party software on external flash
}
