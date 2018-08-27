#include <ion/led.h>
#include <ion/display.h>
#include "device.h"
#include "led.h"
#include "regs/regs.h"

// Public Ion::LED methods

static KDColor sLedColor = KDColorBlack;

KDColor Ion::LED::getColor() {
  return sLedColor;
}

void Ion::LED::setColor(KDColor c) {
  sLedColor = c;

  /* Active all RGB colors */
  TIM3.CCMR()->setOC2M(TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC4M(TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC3M(TIM<Register16>::CCMR::OCM::PWM1);

  /* Set the PWM duty cycles to display the right color */
  constexpr float maxColorValue = (float)((1 << 8) -1);
  Device::setPeriodAndDutyCycles(Device::Mode::PWM, c.red()/maxColorValue, c.green()/maxColorValue, c.blue()/maxColorValue);
}

void Ion::LED::setBlinking(uint16_t period, float dutyCycle) {
  /* We want to use the PWM at a slow rate to display a seeable blink.
   * Consequently, we do not use PWM to display the right color anymore but to
   * blink. We cannot use the PWM to display the exact color so we 'project the
   * color on 3 bits' : all colors have 2 states - active or not. */
  TIM3.CCMR()->setOC2M(sLedColor.red() > 0 ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC4M(sLedColor.green() > 0 ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC3M(sLedColor.blue() > 0 ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);

  Device::setPeriodAndDutyCycles(Device::Mode::Blink, dutyCycle, dutyCycle, dutyCycle, period);
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
  TIM3.CCMR()->setOC2M(TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC4M(TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC3M(TIM<Register16>::CCMR::OCM::ForceInactive);
}

/* Pulse width modulation mode allows you to generate a signal with a
 * frequency determined by the value of the TIMx_ARR register and a duty cycle
 * determined by the value of the TIMx_CCRx register. */

void setPeriodAndDutyCycles(Mode mode, float dutyCycleRed, float dutyCycleGreen, float dutyCycleBlue, uint16_t period) {
  switch (mode) {
    case Mode::PWM:
      /* Let's set the prescaler to 1. Increasing the prescaler would slow down
       * the modulation, which can be useful when debugging or when we want an
       * actual blinking. */
      TIM3.PSC()->set(1);
      TIM3.ARR()->set(PWMPeriod);
      period = PWMPeriod;
      break;
    case Mode::Blink:
      int systemClockFreq = 96;
      /* We still want to do PWM, but at a rate slow enough to blink. Ideally,
       * we want to pre-scale the period to be able to set it in milliseconds;
       * however, as the prescaler is cap by 2^16-1, we divide it by a factor
       * and correct the period consequently. */
      int factor = 2;
      // TODO: explain the 2 ?
      TIM3.PSC()->set(systemClockFreq*1000/factor);
      period *= factor;
      TIM3.ARR()->set(period);
      break;
  }

  TIM3.CCR2()->set(dutyCycleRed*period);
  TIM3.CCR3()->set(dutyCycleBlue*period);
  TIM3.CCR4()->set(dutyCycleGreen*period);
}

}
}
}
