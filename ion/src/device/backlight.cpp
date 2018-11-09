#include <ion.h>
#include "regs/regs.h"
#include "backlight.h"

/* This driver controls the RT9365 LED driver.
 * This chip allows the brightness to be set to 16 different values. It starts
 * at full brightness on power on. Applying a pulse on the EN pin will select
 * the next value in decreasing order. Once it reaches the minimal value, the
 * next pulse will loop back to full brightness. */

// Public Ion::Backlight methods

namespace Ion {
namespace Backlight {

void setBrightness(uint8_t b) {
  Device::setLevel(b >> 4);
}

uint8_t brightness() {
  return Device::level() << 4;
}

}
}

// Private Ion::Backlight::Device methods

namespace Ion {
namespace Backlight {
namespace Device {

static uint8_t sLevel;

void init() {
  GPIOC.MODER()->setMode(6, GPIO::MODER::Mode::Output);
  sLevel = 0xF;
  resume();
}

void shutdown() {
  GPIOC.MODER()->setMode(6, GPIO::MODER::Mode::Analog);
  GPIOC.PUPDR()->setPull(6, GPIO::PUPDR::Pull::None);
}

void suspend() {
  GPIOC.ODR()->set(6, false);
  msleep(3); // Might not need to be blocking
}

void resume() {
  GPIOC.ODR()->set(6, true);
  usleep(50);
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
    GPIOC.ODR()->set(6, false);
    usleep(20);
    GPIOC.ODR()->set(6, true);
    usleep(20);
  }
}

}
}
}
