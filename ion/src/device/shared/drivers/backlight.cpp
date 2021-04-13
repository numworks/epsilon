#include "backlight.h"
#include <drivers/config/backlight.h>
#include <ion/timing.h>

/* This driver controls the RT9365 LED driver.
 * This chip allows the brightness to be set to 16 different values. It starts
 * at full brightness on power on. Applying a pulse on the EN pin will select
 * the next value in decreasing order. Once it reaches the minimal value, the
 * next pulse will loop back to full brightness. */


namespace Ion {
namespace Device {
namespace Backlight {

using namespace Regs;

void initGPIO() {
  Config::BacklightPin.group().MODER()->setMode(Config::BacklightPin.pin(), GPIO::MODER::Mode::Output);
  Config::BacklightPin.group().ODR()->set(Config::BacklightPin.pin(), true);
  Timing::usleep(50);
}

void shutdown() {
  Config::BacklightPin.group().MODER()->setMode(Config::BacklightPin.pin(), GPIO::MODER::Mode::Analog);
  Config::BacklightPin.group().PUPDR()->setPull(Config::BacklightPin.pin(), GPIO::PUPDR::Pull::None);
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
