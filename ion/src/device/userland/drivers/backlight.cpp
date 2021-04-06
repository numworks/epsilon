#include <ion/backlight.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Backlight {

void SVC_ATTRIBUTES init() {
  SVC(SVC_BACKLIGHT_INIT);
}

void SVC_ATTRIBUTES shutdown() {
  SVC(SVC_BACKLIGHT_SHUTDOWN);
}

bool SVC_ATTRIBUTES isInitialized() {
  SVC(SVC_BACKLIGHT_IS_INITIALIZED);
}

void SVC_ATTRIBUTES setBrightness(uint8_t b) {
  SVC(SVC_BACKLIGHT_SET_BRIGHTNESS);
}

uint8_t SVC_ATTRIBUTES brightness() {
  SVC(SVC_BACKLIGHT_BRIGHTNESS);
}

}
}
