#include <drivers/svcall.h>
#include <ion/battery.h>

namespace Ion {
namespace Battery {

bool SVC_ATTRIBUTES isCharging(){
    SVC_RETURNING_R0(SVC_BATTERY_IS_CHARGING, bool)}

Charge SVC_ATTRIBUTES level() {
  SVC_RETURNING_R0(SVC_BATTERY_LEVEL, Charge)
}

float SVC_ATTRIBUTES voltage() { SVC_RETURNING_S0(SVC_BATTERY_VOLTAGE, float) }

}  // namespace Battery
}  // namespace Ion
