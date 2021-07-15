#ifndef ION_DEVICE_SHARED_DRIVERS_LED_H
#define ION_DEVICE_SHARED_DRIVERS_LED_H

#include <stdint.h>
#include <kandinsky/color.h>

namespace Ion {
namespace Device {
namespace LED {

KDColor getColor();
void setColor(KDColor c);
void setBlinking(uint16_t periodInMilliseconds, float dutyCycle);

KDColor updateColorWithPlugAndCharge();

enum class Mode {
  PWM,
  Blink
};

enum class Color {
  Red,
  Green,
  Blue
};

void init();
void shutdown();
void setPeriodAndDutyCycles(Mode mode, float dutyCycleRed, float dutyCycleGreen, float dutyCycleBlue, uint16_t period = 0);

void initGPIO();
void shutdownGPIO();
void initTimer();
void shutdownTimer();

constexpr uint16_t PWMPeriod = 40000;

}
}
}

#endif

