#include <ion/led.h>
#include <kandinsky/basic_color.h>

namespace Ion {
namespace LED {

KDColor getColor() { return BasicColor::Black; }

void setColor(KDColor c) {}

void setBlinking(uint16_t period, float dutyCycle) {}

void setLock(bool) {}

KDColor updateColorWithPlugAndCharge() { return BasicColor::Black; }

}  // namespace LED
}  // namespace Ion
