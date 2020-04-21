#ifndef ION_DRIVER_COMMON_H
#define ION_DRIVER_COMMON_H

#include <ion/battery.h>
#include <time.h>

#define PULL_DELAY 1.0f

namespace Ion {
namespace Simulator {
namespace CommonDriver {

void pullData();
bool isPlugged();
bool isCharging();
Ion::Battery::Charge getLevel();

}
}
}

#endif
