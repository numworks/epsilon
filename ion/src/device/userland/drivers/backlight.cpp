#include <drivers/svcall.h>
#include <ion/backlight.h>

namespace Ion {
namespace Backlight {

void SVC_ATTRIBUTES setBrightness(uint8_t b){
    SVC_RETURNING_VOID(SVC_BACKLIGHT_SET_BRIGHTNESS)}

uint8_t SVC_ATTRIBUTES brightness() {
  SVC_RETURNING_R0(SVC_BACKLIGHT_BRIGHTNESS, uint8_t)
}

}  // namespace Backlight
}  // namespace Ion
