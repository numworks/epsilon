#include <ion/battery.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Battery {

void SVC_ATTRIBUTES isChargingSVC(bool * us) {
  SVC(SVC_BATTERY_IS_CHARGING);
}

bool isCharging() {
  bool res;
  isChargingSVC(&res);
  return res;
}

void SVC_ATTRIBUTES levelSVC(Charge * res) {
  SVC(SVC_BATTERY_LEVEL);
}

Charge level() {
  Charge res;
  levelSVC(&res);
  return res;
}

void SVC_ATTRIBUTES voltageSVC(float * res) {
  SVC(SVC_BATTERY_VOLTAGE);
}

float voltage() {
  float res;
  voltageSVC(&res);
  return res;
}

}
}
