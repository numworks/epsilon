#ifndef ION_SHARED_LED_H
#define ION_SHARED_LED_H

#include <ion/led.h>

namespace Ion {
namespace LED {

void setBlinking(uint16_t periodInMilliseconds, float dutyCycle);
void setLock(bool locked);

}
}

#endif
