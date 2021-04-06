#include <ion/led.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace LED {

KDColor SVC_ATTRIBUTES getColor() {
  SVC(SVC_LED_GET_COLOR);
}

void SVC_ATTRIBUTES setColor(KDColor c) {
  SVC(SVC_LED_SET_COLOR);
}

void SVC_ATTRIBUTES setBlinking(uint16_t periodInMilliseconds, float dutyCycle) {
  SVC(SVC_LED_SET_BLINKING);
}

KDColor SVC_ATTRIBUTES updateColorWithPlugAndCharge() {
  SVC(SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE);
}

}
}
