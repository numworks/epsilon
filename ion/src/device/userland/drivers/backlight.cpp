#include <ion/backlight.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Backlight {

void SVC_ATTRIBUTES init() {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_INIT)
}

void SVC_ATTRIBUTES shutdown() {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_SHUTDOWN)
}

bool SVC_ATTRIBUTES isInitialized() {
  SVC_RETURNING_R0(SVC_BACKLIGHT_IS_INITIALIZED, bool)
}

void SVC_ATTRIBUTES setBrightness(uint8_t b) {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_SET_BRIGHTNESS)
}

uint8_t SVC_ATTRIBUTES brightness() {
  SVC_RETURNING_R0(SVC_BACKLIGHT_BRIGHTNESS, uint8_t)
}

}
}
