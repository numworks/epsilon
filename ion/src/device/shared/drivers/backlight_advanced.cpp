#include "backlight.h"
#include <ion/timing.h>
#include <drivers/config/backlight.h>

namespace Ion {
namespace Device {
namespace Backlight {

// Public Ion::Backlight methods

void setBrightness(uint8_t b) {
  Ion::Device::Backlight::setLevel(b >> 4);
}

uint8_t brightness() {
  return Ion::Device::Backlight::level() << 4;
}

// Private Ion::Backlight::Device methods

using namespace Regs;

constexpr uint8_t k_initialLevel = 0xF;
static uint8_t sLevel = k_initialLevel;

void init() {
  int previousLevel = sLevel;
  initGPIO();
  sLevel = k_initialLevel;
  setLevel(previousLevel);
}

bool isInitialized() {
  return Config::BacklightPin.group().MODER()->getMode(Config::BacklightPin.pin()) == GPIO::MODER::Mode::Output;
}

void setLevel(uint8_t level) {
  // From sLevel = 12 to level 7 : 5 pulses
  // From sLevel = 5 to level 9 : 12 pulses (5 to go to level 16, and 7 to 9)
  if (sLevel < level) {
    sendPulses(16 + sLevel - level);
  } else {
    sendPulses(sLevel - level);
  }
  sLevel = level;
}

uint8_t level() {
  return sLevel;
}

}
}
}

