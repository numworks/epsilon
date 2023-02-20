#ifndef ION_LED_H
#define ION_LED_H

#include <kandinsky/color.h>

namespace Ion {
namespace LED {

KDColor getColor();
void setColor(KDColor c);

KDColor updateColorWithPlugAndCharge();

}  // namespace LED
}  // namespace Ion

#endif
