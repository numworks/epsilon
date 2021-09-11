#include "battery.h"
#include <ion/battery.h>
#include <drivers/config/battery.h>

/* To measure the battery voltage, we're using the internal ADC. The ADC works
 * by comparing the input voltage to a reference voltage. The only fixed voltage
 * we have around is 2.8V, so that's the one we're using as a refrence. However,
 * and ADC can only measure voltage that is lower than the reference voltage. So
 * we need to use a voltage divider before sampling Vbat.
 * To avoid draining the battery, we're using an high-impedence voltage divider,
 * so we need to be careful when sampling the ADC. See AN2834 for more info. */


namespace Ion {
namespace Battery {

using namespace Device::Battery;
using namespace Device::Regs;

bool isCharging() {
  return !Config::ChargingPin.group().IDR()->get(Config::ChargingPin.pin());
}

Charge level() {
  static Charge previousLevel = Charge::FULL;


  // Get the current voltage
  float currentVoltage = voltage();

  constexpr static int numberOfChargeStates = 4;
  constexpr static int numberOfThresholds = numberOfChargeStates - 1;
  constexpr float hysteresis = 0.04f;
  const float thresholds[numberOfThresholds] = {3.6f + hysteresis, 3.7f, 3.8f}; // We do not want to lower the threshold for empty battery, so we add the hysteresis to it
  int nextLevel = -1;
  for (int i = 0; i < numberOfThresholds; i++) {
    if (currentVoltage < thresholds[i] + hysteresis * (i < (int)previousLevel ? -1.0f : 1.0f)) {
      nextLevel = i;
      break;
    }
  }
  if (nextLevel < 0) {
    nextLevel = (int) Charge::FULL;
  }
  previousLevel = (Charge)nextLevel;
  return previousLevel;
}

float voltage() {
  ADC.CR2()->setSWSTART(true);
  while (ADC.SR()->getEOC() != true) {
  }
  uint16_t value = ADC.DR()->get();

  // The ADC is 12 bits by default
  return Config::ADCDividerBridgeRatio*(Config::ADCReferenceVoltage * value)/0xFFF;
}

}
}

namespace Ion {
namespace Device {
namespace Battery {

using namespace Regs;

void init() {
  initGPIO();

  /* The BAT_SNS pin is connected to Vbat through a divider bridge. It therefore
   * has a voltage of Vbat/2. We'll measure this using ADC channel 0. */
  Config::ADCPin.group().MODER()->setMode(Config::ADCPin.pin(), GPIO::MODER::Mode::Analog);

  // Step 2 - Enable the ADC
  RCC.APB2ENR()->setADC1EN(true);
  ADC.CR2()->setADON(true);

  // Configure the ADC channel
  ADC.SQR1()->setL(0); // Always sample the same channel
  ADC.SQR3()->setSQ1(Config::ADCChannel);
  ADC.SMPR()->setSamplingTime(Config::ADCChannel, ADC::SMPR::SamplingTime::Cycles480); // Use the max sampling time
}

void initGPIO() {
  /* Step 1 - Configure the GPIOs
   * The BAT_CHRG pin is connected to the Li-Po charging IC. That pin uses an
   * open-drain output. Open-drain output are either connected to ground or left
   * floating. To interact with such an output, our input must therefore be
   * pulled up. */
  Config::ChargingPin.group().MODER()->setMode(Config::ChargingPin.pin(), GPIO::MODER::Mode::Input);
  Config::ChargingPin.group().PUPDR()->setPull(Config::ChargingPin.pin(), GPIO::PUPDR::Pull::Up);
}

void shutdown() {
  Config::ChargingPin.group().MODER()->setMode(Config::ChargingPin.pin(), GPIO::MODER::Mode::Analog);
  Config::ChargingPin.group().PUPDR()->setPull(Config::ChargingPin.pin(), GPIO::PUPDR::Pull::None);

  // Disable the ADC
  ADC.CR2()->setADON(false);
  RCC.APB2ENR()->setADC1EN(false);
}

}
}
}
