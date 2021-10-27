#include <ion/battery.h>
#include <ion/battery.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Battery {

bool SVC_ATTRIBUTES isCharging() {
  SVC_RETURNING_R0(SVC_BATTERY_IS_CHARGING, bool)
}

Charge SVC_ATTRIBUTES level() {
  SVC_RETURNING_R0(SVC_BATTERY_LEVEL, Charge)
}

uint32_t SVC_ATTRIBUTES voltage_int() {
  SVC_RETURNING_R0(SVC_BATTERY_VOLTAGE, uint32_t);
}

float voltage() {
  union {
    volatile float f;
    volatile uint32_t i;
  };
  i = voltage_int();
  return f;
}

}
}
