#include <drivers/config/display.h>
#include <shared/drivers/display.h>

namespace Ion {
namespace Device {
namespace Display {

bool waitForVBlank() {
  /* Simple version of waitForVBlank which does not require systick.
   * Min screen frequency is 40Hz so the maximal period is T = 1/40Hz = 25ms.
   * If after T ms, we still do not have a VBlank event, just return.
   * We clock at 192 MHz and each loop is about 10 instructions. So waiting
   * 50 ms corresponds to looping 50 ms * 192 MHz / 10 */
  constexpr int numberOfTries = 9600000;

  /* We want to return at the beginning of a high signal, so we wait for the
   * signal to be low then high. */
  bool wasLow = false;

  for (int i = 0; i < numberOfTries; i++) {
    if (!wasLow) {
      wasLow = !Config::TearingEffectPin.group().IDR()->get(Config::TearingEffectPin.pin());
    }
    if (wasLow) {
      if (Config::TearingEffectPin.group().IDR()->get(Config::TearingEffectPin.pin())) {
        return true;
      }
    }
  }
  return false;
}

}
}
}
