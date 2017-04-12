#include <ion/led.h>
#include <ion/display.h>
#include "led.h"
#include "regs/regs.h"

// Public Ion::LED methods

void Ion::LED::setColor(KDColor c) {
  TIM3.CCR2()->set(Device::dutyCycleForUInt8(c.red()));
  TIM3.CCR3()->set(Device::dutyCycleForUInt8(c.blue()));
  TIM3.CCR4()->set(Device::dutyCycleForUInt8(c.green()));
}

// Private Ion::Device::LED methods

namespace Ion {
namespace LED {
namespace Device {

void init() {
  initGPIO();
  initTimer();
}

void shutdown() {
  shutdownTimer();
  shutdownGPIO();
}

void initGPIO() {
  /* RED_LED(PC7), GREEN_LED(PB1), and BLUE_LED(PB0) are driven using a timer,
   * which is an alternate function. More precisely, we will use AF2, which maps
   * PB0 to TIM3_CH2, PB1 to TIM3_CH4, and PC7 to TIM3_CH2. */
  for(const GPIOPin & g : RGBPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF2);
  }
}

void shutdownGPIO() {
  for(const GPIOPin & g : RGBPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

void initTimer() {
  /* Let's set the prescaler to 1. Increasing the prescaler would slow down the
   * modulation, which can be useful when debugging. */
  TIM3.PSC()->set(1);

  /* Pulse width modulation mode allows you to generate a signal with a
   * frequency determined by the value of the TIMx_ARR register and a duty cycle
   * determined by the value of the TIMx_CCRx register. */
  TIM3.ARR()->set(PWMPeriod);
  TIM3.CCR2()->set(0);
  TIM3.CCR3()->set(0);
  TIM3.CCR4()->set(0);

  // Set Channels 2-4 as outputs, PWM mode 1
  TIM3.CCMR()->setOC2M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC3M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC4M(TIM::CCMR::OCM::PWM1);

  // Output preload enable for channels 2-4
  TIM3.CCMR()->setOC2PE(true);
  TIM3.CCMR()->setOC3PE(true);
  TIM3.CCMR()->setOC4PE(true);

  // Auto-reload preload enable
  TIM3.CR1()->setARPE(true);

  // Enable Capture/Compare for channel 2 to 4
  TIM3.CCER()->setCC2E(true);
  TIM3.CCER()->setCC3E(true);
  TIM3.CCER()->setCC4E(true);

  TIM3.BDTR()->setMOE(true);

  TIM3.CR1()->setCEN(true);
}

void shutdownTimer() {
  TIM3.CCMR()->setOC2M(TIM::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC3M(TIM::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC4M(TIM::CCMR::OCM::ForceInactive);
}

void enforceState(bool red, bool green, bool blue) {
  bool states[3] = {red, green, blue};
  for (int i=0; i<3; i++) {
    GPIOPin p = RGBPins[i];
    if (states[i]) {
      p.group().MODER()->setMode(p.pin(), GPIO::MODER::Mode::Output);
      p.group().ODR()->set(p.pin(), true);
    } else {
      p.group().MODER()->setMode(p.pin(), GPIO::MODER::Mode::Analog);
      p.group().PUPDR()->setPull(p.pin(), GPIO::PUPDR::Pull::None);
    }
  }
}

}
}
}
