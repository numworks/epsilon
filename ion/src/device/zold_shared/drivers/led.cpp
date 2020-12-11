#include "led.h"
#include <ion/led.h>
#include <drivers/config/led.h>
#include <drivers/config/clocks.h>

static KDColor sLedColor = KDColorBlack;

namespace Ion {
namespace LED {

using namespace Device::LED;
using namespace Device::Regs;

KDColor getColor() {
  return sLedColor;
}

void setColor(KDColor c) {
  sLedColor = c;

  /* Active all RGB colors */
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::RedChannel, TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::GreenChannel, TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::BlueChannel, TIM<Register16>::CCMR::OCM::PWM1);

  /* Set the PWM duty cycles to display the right color */
  constexpr float maxColorValue = (float)((1 << 8) -1);
  setPeriodAndDutyCycles(Mode::PWM, c.red()/maxColorValue, c.green()/maxColorValue, c.blue()/maxColorValue);
}

void setBlinking(uint16_t period, float dutyCycle) {
  /* We want to use the PWM at a slow rate to display a seeable blink.
   * Consequently, we do not use PWM to display the right color anymore but to
   * blink. We cannot use the PWM to display the exact color so we 'project the
   * color on 3 bits' : all colors have 2 states - active or not. */
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::RedChannel, sLedColor.red() > 0 ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::GreenChannel, sLedColor.green() > 0 ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::BlueChannel, sLedColor.blue() > 0 ? TIM<Register16>::CCMR::OCM::PWM1 : TIM<Register16>::CCMR::OCM::ForceInactive);

  setPeriodAndDutyCycles(Mode::Blink, dutyCycle, dutyCycle, dutyCycle, period);
}

}
}

namespace Ion {
namespace Device {
namespace LED {

using namespace Regs;

void init() {
  initGPIO();
  initTimer();
}

void shutdown() {
  shutdownTimer();
  shutdownGPIO();
}

void initGPIO() {
  /* RED_LED, GREEN_LED, and BLUE_LED are driven using a timer, which is an
   * alternate function. More precisely, we will use AF2, which maps each GPIO
   * to a TIM3_CH. */
  for(const AFGPIOPin & p : Config::RGBPins) {
    p.init();
  }
}

void shutdownGPIO() {
  for(const AFGPIOPin & p : Config::RGBPins) {
    p.shutdown();
  }
}

void initTimer() {
  // Output preload enable
  TIM3.CCMR()->setOCPE(Ion::Device::LED::Config::RedChannel, true);
  TIM3.CCMR()->setOCPE(Ion::Device::LED::Config::GreenChannel, true);
  TIM3.CCMR()->setOCPE(Ion::Device::LED::Config::BlueChannel, true);

  // Auto-reload preload enable
  TIM3.CR1()->setARPE(true);

  // Enable Capture/Compare for channel 2 to 4
  TIM3.CCER()->setCCE(Ion::Device::LED::Config::RedChannel, true);
  TIM3.CCER()->setCCE(Ion::Device::LED::Config::GreenChannel, true);
  TIM3.CCER()->setCCE(Ion::Device::LED::Config::BlueChannel, true);

  TIM3.BDTR()->setMOE(true);

  TIM3.CR1()->setCEN(true);
}

void shutdownTimer() {
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::RedChannel, TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::GreenChannel, TIM<Register16>::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::BlueChannel, TIM<Register16>::CCMR::OCM::ForceInactive);
}

/* Pulse width modulation mode allows you to generate a signal with a
 * frequency determined by the value of the TIMx_ARR register and a duty cycle
 * determined by the value of the TIMx_CCRx register. */

void setPeriodAndDutyCycles(Mode mode, float dutyCycleRed, float dutyCycleGreen, float dutyCycleBlue, uint16_t period) {
  switch (mode) {
    case Mode::PWM:
      /* Let's set the prescaler to 1 (PSC = 0). Increasing the prescaler would slow down
       * the modulation, which can be useful when debugging or when we want an
       * actual blinking. */
      TIM3.PSC()->set(0);
      TIM3.ARR()->set(PWMPeriod);
      period = PWMPeriod;
      break;
    case Mode::Blink:
      /* We still want to do PWM, but at a rate slow enough to blink. Ideally,
       * we want to pre-scale the period to be able to set it in milliseconds;
       * however, as the prescaler is cap by 2^16-1, we divide it by a factor
       * and correct the period consequently. */
      int prescalerFactor = 4;
      /* Most of the time AP1TimerFrequency is the 'low' one as most of the time
       * is spent in Timing::msleep. */
      TIM3.PSC()->set(Clocks::Config::APB1TimerLowFrequency*1000/prescalerFactor-1);
      period *= prescalerFactor;
      TIM3.ARR()->set(period);
      break;
  }

  TIM3.CCR(Ion::Device::LED::Config::RedChannel)->set(dutyCycleRed*period);
  TIM3.CCR(Ion::Device::LED::Config::GreenChannel)->set(dutyCycleGreen*period);
  TIM3.CCR(Ion::Device::LED::Config::BlueChannel)->set(dutyCycleBlue*period);
}

}
}
}
