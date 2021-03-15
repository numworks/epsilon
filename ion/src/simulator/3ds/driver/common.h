#ifndef ION_DRIVER_COMMON_H
#define ION_DRIVER_COMMON_H

#include <ion/battery.h>
#include <time.h>
#include <3ds.h>

#define PULL_DELAY 1.0f

typedef struct {
  u32 ani;
  u8 r[32];
  u8 g[32];
  u8 b[32];
} RGBLedPattern;

namespace Ion {
namespace Simulator {
namespace CommonDriver {

void init();
void deinit();
void pullData();
bool isPlugged();
bool isCharging();
Ion::Battery::Charge getLevel();
Result common_ptmsysmSetInfoLedPattern(RGBLedPattern pattern);

}
}
}

#endif
