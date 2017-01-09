#include <ion/battery.h>
#include "battery.h"
#include "regs/regs.h"

/* To measure the battery voltage, we're using the internal ADC. The ADC works
 * by comparing the input voltage to a reference voltage. The only fixed voltage
 * we have around is 2.8V, so that's the one we're using as a refrence. However,
 * and ADC can only measure voltage that is lower than the reference voltage. So
 * we need to use a voltage divider before sampling Vbat.
 * To avoid draining the battery, we're using an high-impedence voltage divider,
 * so we need to be careful when sampling the ADC. See AN2834 for more info. */

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
  initGPIO();
  initADC();
}

void initGPIO() {
  /* The BAT_CHRG pin is connected to the Li-Po charging IC. That pin uses an
   * open-drain output. Open-drain output are either connected to ground or left
   * floating. To interact with such an output, our input must therefore be
   * pulled up. */
  ChargingGPIO.MODER()->setMode(ChargingPin, GPIO::MODER::Mode::Input);
  ChargingGPIO.PUPDR()->setPull(ChargingPin, GPIO::PUPDR::Pull::Up);

  /* The BAT_SNS pin is connected to Vbat through a divider bridge. It therefore
   * has a voltage of Vbat/2. We'll measure this using ADC channel 0. */
  ADCGPIO.MODER()->setMode(ADCPin, GPIO::MODER::Mode::Analog);

  ADC.SMPR()->setSamplingTime(ADCChannel, ADC::SMPR::SamplingTime::Cycles480);
  // ADC.SQR1()->setL(0); // Default
  ADC.SQR3()->setSQ1(ADCChannel);
}

void initADC() {
  ADC.CR2()->setADON(true);
}

}
}
}
