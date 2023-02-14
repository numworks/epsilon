#ifndef ION_LED_H
#define ION_LED_H

#include <kandinsky/color.h>

namespace Ion {
namespace LED {

KDColor getColor();
void setColor(KDColor c);
void setBlinking(uint16_t periodInMilliseconds, float dutyCycle);
void setLock(bool locked);

KDColor updateColorWithPlugAndCharge();

}  // namespace LED
}  // namespace Ion

#endif
