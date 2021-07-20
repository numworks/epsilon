#include <drivers/config/display.h>
#include <ion/timing.h>
#include <kernel/drivers/board.h>
#include <shared/drivers/display.h>

namespace Ion {
namespace Device {
namespace Display {

bool waitForVBlank() {
  // IDEA : implémenter sans millis?
  /* Min screen frequency is 40Hz so the maximal period is T = 1/40Hz = 25ms.
   * If after T ms, we still do not have a VBlank event, just return. */
  constexpr uint64_t timeoutDelta = 50;
  uint64_t startTime = Timing::millis();
  uint64_t timeout = startTime + timeoutDelta;

  /* If current time is big enough, currentTime + timeout wraps around the
   * uint64_t. We need to take this into account when computing the terminating
   * event.
   *
   * NO WRAP |----------------|++++++++++|------|
   *         0           startTime    timeout   max uint64_t
   *
   * WRAP    |++++|----------------------|++++++|
   *         0  timeout              startTime  max uint64_t
   */
  bool noWrap = startTime < timeout;

  /* We want to return at the beginning of a high signal, so we wait for the
   * signal to be low then high. */
  bool wasLow = false;

  uint64_t currentTime = startTime;
  while (noWrap ?
      (currentTime >= startTime && currentTime < timeout) :
      (currentTime >= startTime || currentTime < timeout))
  {
    if (!wasLow) {
      wasLow = !Config::TearingEffectPin.group().IDR()->get(Config::TearingEffectPin.pin());
    }
    if (wasLow) {
      if (Config::TearingEffectPin.group().IDR()->get(Config::TearingEffectPin.pin())) {
        return true;
      }
    }
    currentTime = Timing::millis();
  }
  return false;
}

void pullRectSecure(KDRect r, KDColor * pixels) {
  if (Board::addressInUserlandRAM(pixels)) {
    pullRect(r, pixels);
  }
}

}
}
}
