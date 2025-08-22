#pragma once

#include <ion/led.h>

namespace Ion {
namespace LED {

void setBlinking(uint16_t periodInMilliseconds, float dutyCycle);
void setLock(bool locked);

}  // namespace LED
}  // namespace Ion
