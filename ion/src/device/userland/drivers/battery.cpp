#include <ion/battery.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Battery {

bool SVC_ATTRIBUTES isCharging() {
  SVC(SVC_BATTERY_IS_CHARGING);
}

Charge SVC_ATTRIBUTES level() {
  SVC(SVC_BATTERY_LEVEL);
}

float SVC_ATTRIBUTES voltage() {
  SVC(SVC_BATTERY_VOLTAGE);
}

}
}
