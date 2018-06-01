#include "device.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}
#include <ion.h>
#include "display.h"


// Public Ion methods

void Ion::msleep(long ms) {
  if (ms == 0) {
    return;
  }

  PWM.TCNTB0()->set(ms*33);

  PWM.TCON()->setTimer(0, PWM::TCON::MANUAL_UPDATE);
  PWM.TCON()->setTimer(0, PWM::TCON::ENABLE);

  while (PWM.TCNTO0()->get()) {
    __asm volatile("nop");
  }
}

void Ion::usleep(long us) {
  // XXX: calibrate loop
  for (volatile long i=0; i<25*us; i++) {
    __asm volatile("nop");
  }
}

// Private Ion::Device methods

namespace Ion {
namespace Device {

void init() {
  // TIM0 keeps track of time
  PWM.TCFG0()->setPRESCALER0(124);
  PWM.TCFG0()->setPRESCALER1(124);
  PWM.TCFG1()->setMode(0, PWM::TCFG1::Mode::MUX_1_16);

  initPeripherals();
}

void initPeripherals() {
  Ion::Display::Device::init();
}

}
}
