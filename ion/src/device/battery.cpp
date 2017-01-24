#include <ion/battery.h>
#include "battery.h"
#include "regs/regs.h"

namespace Ion {
namespace Battery {

bool isCharging() {
  return !Device::ChargingGPIO.IDR()->get(Device::ChargingPin);
}

float voltage() {
  ADC.CR2()->setSWSTART(true);
  while (ADC.SR()->getEOC() != true) {
  }
  uint16_t value = ADC.DR()->get();

  // The ADC is 12 bits by default
  return Device::ADCDividerBridgeRatio*(Device::ADCReferenceVoltage * value)/0xFFF;
}


}
}

namespace Ion {
namespace Battery {
namespace Device {

void init() {
  /* Step 1 - Configure the GPIOs
   * The BAT_CHRG pin is connected to the Li-Po charging IC. That pin uses an
   * open-drain output. Open-drain output are either connected to ground or left
   * floating. To interact with such an output, our input must therefore be
   * pulled up. */
  ChargingGPIO.MODER()->setMode(ChargingPin, GPIO::MODER::Mode::Input);
  ChargingGPIO.PUPDR()->setPull(ChargingPin, GPIO::PUPDR::Pull::Up);

  /* The BAT_SNS pin is connected to Vbat through a divider bridge. It therefore
   * has a voltage of Vbat/2. We'll measure this using ADC channel 0. */
  ADCGPIO.MODER()->setMode(ADCPin, GPIO::MODER::Mode::Analog);

  // Step 2 - Enable the ADC
  RCC.APB2ENR()->setADC1EN(true);
  ADC.CR2()->setADON(true);
}

void shutdown() {
  // Disable the ADC
  ADC.CR2()->setADON(false);
  RCC.APB2ENR()->setADC1EN(false);
}

}
}
}
