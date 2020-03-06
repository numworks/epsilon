#ifndef ION_BACKLIGHT_H
#define ION_BACKLIGHT_H

#include <stdint.h>

namespace Ion {
namespace Backlight {

constexpr uint8_t MaxBrightness = 240;
void init();
bool isInitialized();
void shutdown();
void setBrightness(uint8_t b);
uint8_t brightness();

}
}

#endif
