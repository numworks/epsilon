#include <ion/led.h>

namespace Ion {
namespace LED {

KDColor getColor() { return KDColorBlack; }

void setColor(KDColor c) {}

void setBlinking(uint16_t period, float dutyCycle) {}

KDColor updateColorWithPlugAndCharge() { return KDColorBlack; }

}  // namespace LED
}  // namespace Ion
