#include "backlight.h"
#include <ion/backlight.h>
#include <ion/timing.h>
#include <drivers/config/backlight.h>

/* This driver controls the RT9365 LED driver.
 * This chip allows the brightness to be set to 16 different values. It starts
 * at full brightness on power on. Applying a pulse on the EN pin will select
 * the next value in decreasing order. Once it reaches the minimal value, the
 * next pulse will loop back to full brightness. */

// Public Ion::Backlight methods

namespace Ion {
namespace Backlight {

void setBrightness(uint8_t b) {
  Ion::Device::Backlight::setLevel(b >> 4);
}

uint8_t brightness() {
  return Ion::Device::Backlight::level() << 4;
}

void init() {
  Ion::Device::Backlight::init();
}

bool isInitialized() {
  return Ion::Device::Backlight::isInitialized();
}

void shutdown() {
  Ion::Device::Backlight::shutdown();
}

}
}

// Private Ion::Backlight::Device methods

namespace Ion {
namespace Device {
namespace Backlight {

using namespace Regs;

static uint8_t sLevel;

void init() {
  Config::BacklightPin.group().MODER()->setMode(Config::BacklightPin.pin(), GPIO::MODER::Mode::Output);
  sLevel = 0xF;
  resume();
}

bool isInitialized() {
  return Config::BacklightPin.group().MODER()->getMode(Config::BacklightPin.pin()) == GPIO::MODER::Mode::Output;
}

void shutdown() {
  Config::BacklightPin.group().MODER()->setMode(Config::BacklightPin.pin(), GPIO::MODER::Mode::Analog);
  Config::BacklightPin.group().PUPDR()->setPull(Config::BacklightPin.pin(), GPIO::PUPDR::Pull::None);
}

void suspend() {
  Config::BacklightPin.group().ODR()->set(Config::BacklightPin.pin(), false);
  Timing::msleep(3); // Might not need to be blocking
}

void resume() {
  Config::BacklightPin.group().ODR()->set(Config::BacklightPin.pin(), true);
  Timing::usleep(50);
  uint8_t level = sLevel;
  sLevel = 0xF;
  setLevel(level);
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

void sendPulses(int n) {
  for (int i=0; i<n; i++) {
    Config::BacklightPin.group().ODR()->set(Config::BacklightPin.pin(), false);
    Timing::usleep(20);
    Config::BacklightPin.group().ODR()->set(Config::BacklightPin.pin(), true);
    Timing::usleep(20);
  }
}

}
}
}
