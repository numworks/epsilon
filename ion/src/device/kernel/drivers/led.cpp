#include <drivers/led.h>
#include <kernel/drivers/authentication.h>
#include <drivers/config/led.h>
#include <drivers/config/clocks.h>

namespace Ion {
namespace Device {
namespace LED {

using namespace Regs;

extern KDColor sLedColor;

void setColor(KDColor c) {
  if (Authentication::clearanceLevel() != Ion::Authentication::ClearanceLevel::NumWorks) {
    sLedColor = KDColorBlack;
    return;
  }
  sLedColor = c;

  /* Active all RGB colors */
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::RedChannel, TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::GreenChannel, TIM<Register16>::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOCM(Ion::Device::LED::Config::BlueChannel, TIM<Register16>::CCMR::OCM::PWM1);

  /* Set the PWM duty cycles to display the right color */
  constexpr float maxColorValue = (float)((1 << 8) -1);
  setPeriodAndDutyCycles(Mode::PWM, c.red()/maxColorValue, c.green()/maxColorValue, c.blue()/maxColorValue);
}

}
}
}
