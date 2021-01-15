#include <ion/led.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace LED {

void SVC_ATTRIBUTES getColorSVC(KDColor * res) {
  SVC(SVC_LED_GET_COLOR);
}

KDColor getColor() {
  KDColor res;
  getColorSVC(&res);
  return res;
}

void SVC_ATTRIBUTES setColorSVC(KDColor * s) {
  SVC(SVC_LED_SET_COLOR);
}

void setColor(KDColor c) {
  setColorSVC(&c);
}

void SVC_ATTRIBUTES setBlinkingSVC(uint16_t * periodInMilliseconds, float * dutyCycle) {
  SVC(SVC_LED_SET_BLINKING);
}

void setBlinking(uint16_t periodInMilliseconds, float dutyCycle) {
  setBlinkingSVC(&periodInMilliseconds, &dutyCycle);
}

void SVC_ATTRIBUTES updateColorWithPlugAndChargeSVC(KDColor * res) {
  SVC(SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE);
}

KDColor updateColorWithPlugAndCharge() {
  KDColor res;
  updateColorWithPlugAndChargeSVC(&res);
  return res;
}

}
}
