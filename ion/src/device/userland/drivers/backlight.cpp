#include <ion/backlight.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Backlight {

void init() {
  SVC(SVC_BACKLIGHT_INIT);
}

void shutdown() {
  SVC(SVC_BACKLIGHT_SHUTDOWN);
}

void SVC_ATTRIBUTES isInitializedSVC(bool * res) {
  SVC(SVC_BACKLIGHT_IS_INITIALIZED);
}

bool isInitialized() {
  bool res;
  isInitializedSVC(&res);
  return res;
}

void SVC_ATTRIBUTES setBrightnessSVC(uint8_t * us) {
  SVC(SVC_BACKLIGHT_SET_BRIGHTNESS);
}

void setBrightness(uint8_t b) {
  setBrightnessSVC(&b);
}

void SVC_ATTRIBUTES brightnessSVC(uint8_t * b) {
  SVC(SVC_BACKLIGHT_BRIGHTNESS);
}

uint8_t brightness() {
  uint8_t b;
  brightnessSVC(&b);
  return b;
}

}
}
