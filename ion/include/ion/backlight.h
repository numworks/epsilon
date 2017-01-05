#ifndef ION_BACKLIGHT_H
#define ION_BACKLIGHT_H

#include <stdint.h>

namespace Ion {
namespace Backlight {

void setBrightness(uint8_t b);
uint8_t brightness();

}
}

#endif
