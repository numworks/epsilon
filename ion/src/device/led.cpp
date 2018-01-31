#include <ion/led.h>
#include <ion/display.h>
#include "device.h"
#include "led.h"
#include "regs/regs.h"

// Public Ion::LED methods

void Ion::LED::setColor(KDColor c) {
  if (getLockState()) {
    return;
  }

  initTimer(Ion::LED::Device::Mode::PWM);

  Ion::LED::Device::setColorStatus(Ion::LED::Device::Color::RED, true);
  Ion::LED::Device::setColorStatus(Ion::LED::Device::Color::GREEN, true);
  Ion::LED::Device::setColorStatus(Ion::LED::Device::Color::BLUE, true);

  TIM3.CCR2()->set(Device::dutyCycleForUInt8(c.red()));
  TIM3.CCR3()->set(Device::dutyCycleForUInt8(c.blue()));
  TIM3.CCR4()->set(Device::dutyCycleForUInt8(c.green()));
}

void Ion::LED::setBlinking(float blinkPeriod, bool red, bool green, bool blue) {
  if (getLockState()) {
    return;
  }

  initTimer(Ion::LED::Device::Mode::BLINK, blinkPeriod);

  Ion::LED::Device::setColorStatus(Ion::LED::Device::Color::RED, red);
  Ion::LED::Device::setColorStatus(Ion::LED::Device::Color::GREEN, green);
  Ion::LED::Device::setColorStatus(Ion::LED::Device::Color::BLUE, blue);
}

void Ion::LED::setCharging(bool isPluggedIn, bool isCharging) {
  if (!isPluggedIn) {
    Ion::LED::setColor(KDColorBlack);
  }
  else {
    if (isCharging)  {
      Ion::LED::setColor(KDColorRed);
    }
    else {
      Ion::LED::setColor(KDColorGreen);
    }
  }
}

static bool lockState = false;

bool Ion::LED::getLockState() {
  return lockState;
}

void Ion::LED::setLockState(bool state) {
  lockState = state;
}

// Private Ion::Device::LED methods

namespace Ion {
namespace LED {
namespace Device {

void init() {
  initGPIO();
}

void shutdown() {
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

void initTimer(Mode mode, float blinkPeriod) {
  constexpr int TIM3_FREQ = 4*1000;

  /* Pulse width modulation mode allows you to generate a signal with a
   * frequency determined by the value of the TIMx_ARR register and a duty cycle
   * determined by the value of the TIMx_CCRx register. */
  switch (mode) {
    case Ion::LED::Device::Mode::PWM:
      /* Let's set the prescaler to 1. Increasing the prescaler would slow down the
      * modulation, which can be useful when debugging. */
      TIM3.PSC()->set(1);
      TIM3.ARR()->set(PWMPeriod);
      TIM3.CCR2()->set(0);
      TIM3.CCR3()->set(0);
      TIM3.CCR4()->set(0);
      break;
    case Ion::LED::Device::Mode::BLINK:
      /* We still want to do PWM, but at a rate slow enough to blink. */
      TIM3.PSC()->set(Ion::Device::SYSBUS_FREQ / TIM3_FREQ);
      TIM3.ARR()->set(blinkPeriod * TIM3_FREQ);
      TIM3.CCR2()->set(blinkPeriod * TIM3_FREQ / 2);
      TIM3.CCR3()->set(blinkPeriod * TIM3_FREQ / 2);
      TIM3.CCR4()->set(blinkPeriod * TIM3_FREQ / 2);
      break;
  }

  // Set Channels 2-4 as outputs, PWM mode 1
  TIM3.CCMR()->setOC2M(TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC3M(TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC4M(TIM<Register16>::CCMR::OCM::PWM1);

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

void initTimerColor() {
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
  setColorStatus(Ion::LED::Device::Color::RED, false);
  setColorStatus(Ion::LED::Device::Color::GREEN, false);
  setColorStatus(Ion::LED::Device::Color::BLUE, false);
}

void setColorStatus(Color color, bool enable) {
  switch (color) {
    case Ion::LED::Device::Color::RED:
      TIM3.CCMR()->setOC2M(enable ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);
      break;
    case Ion::LED::Device::Color::GREEN:
      TIM3.CCMR()->setOC4M(enable ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);
      break;
    case Ion::LED::Device::Color::BLUE:
      TIM3.CCMR()->setOC3M(enable ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);
      break;
  }
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
