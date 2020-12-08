#include <ion/display.h>
#include <ion/timing.h>
#include <drivers/config/display.h>
#include <assert.h>

/* This driver interfaces with the ST7789V LCD controller.
 * This chip keeps a whole frame in SRAM memory and feeds it to the LCD panel as
 * needed. We use the STM32's FSMC to drive the bus between the ST7789V. Once
 * configured, we only need to write in the address space of the MCU to actually
 * send some data to the LCD controller. */

#define USE_DMA_FOR_PUSH_PIXELS 0
#define USE_DMA_FOR_PUSH_COLOR 0

#define USE_DMA (USE_DMA_FOR_PUSH_PIXELS|USE_DMA_FOR_PUSH_COLOR)

namespace Ion {
namespace Display {

using namespace Device::Display;

bool waitForVBlank() {
  /* Min screen frequency is 40Hz so the maximal period is T = 1/40Hz = 25ms.
   * If after T ms, we still do not have a VBlank event, just return. */
  constexpr uint64_t timeoutDelta = 50;
  uint64_t startTime = Timing::millis();
  uint64_t timeout = startTime + timeoutDelta;

  /* If current time is big enough, currentTime + timeout wraps aroud the
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
    // TODO: sleep?
  }
  return false;
}

int displayUniformTilingSize10(KDColor c) {
  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");

  {
    KDColor stamp[stampWidth*stampHeight];
    for (int i=0;i<stampWidth*stampHeight; i++) {
      stamp[i] = c;
    }

    for (int i=0; i<Ion::Display::Width/stampWidth; i++) {
      for (int j=0; j<Ion::Display::Height/stampHeight; j++) {
        Ion::Display::pushRect(KDRect(i*stampWidth, j*stampHeight, stampWidth, stampHeight), stamp);
      }
    }
  }

  int numberOfInvalidPixels = 0;
  constexpr int stampHeightPull = 80;
  constexpr int stampWidthPull = 80;
  KDColor stamp[stampHeightPull*stampHeightPull];

  for (int i=0; i<Ion::Display::Width/stampWidthPull; i++) {
    for (int j=0; j<Ion::Display::Height/stampHeightPull; j++) {
      for (int k=0; k<stampWidthPull*stampHeightPull; k++) {
        stamp[k] = KDColorBlack;
      }
      Ion::Display::pullRect(KDRect(i*stampWidthPull, j*stampHeightPull, stampWidthPull, stampHeightPull), stamp);
      for (int k=0; k<stampWidthPull*stampHeightPull; k++) {
        if (stamp[k] != c) {
          numberOfInvalidPixels++;
          break;
        }
      }
    }
  }

  return numberOfInvalidPixels;
}

int displayColoredTilingSize10() {
  /* Draw a tiled pattern:
   * On first pass, red / blue / red / blue...
   * On second pass, blue / green / blue / green...
   * On third pass, green / red / green / red...
   * And so on. */

  constexpr int stampSize = 10;
  constexpr int numberOfStamps = 3;
  static_assert(Ion::Display::Width % stampSize == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampSize == 0, "Stamps must tesselate the display");

  KDColor stamps[numberOfStamps][stampSize*stampSize];
  constexpr KDColor colorForStamp[numberOfStamps] = {KDColorRed, KDColorBlue, KDColorGreen};
  for (int i=0; i<numberOfStamps; i++) {
    KDColor c = colorForStamp[i];
    KDColor * stamp = stamps[i];
    for (int j=0; j<stampSize*stampSize; j++) {
      stamp[j] = c;
    }
  }

  int numberOfHorizontalTiles = Ion::Display::Width / stampSize;
  int numberOfVerticalTiles = Ion::Display::Height / stampSize;

  constexpr int numberOfPasses = 150;
  int numberOfInvalidPixels = 0;

  constexpr int stampHeightPull = 1;
  constexpr int stampWidthPull = 1;
  KDColor resultStamp[stampHeightPull*stampHeightPull];

  for (int p=0; p<numberOfPasses; p++) {
    // Push a checker pattern on the screen
    int firstColorIndex = p%numberOfStamps;
    int secondColorIndex = (p+1)%numberOfStamps;
    KDColor * firstStamp = stamps[firstColorIndex];
    KDColor * secondStamp = stamps[secondColorIndex];
    // Draw the pattern
    for (int j=0; j<numberOfVerticalTiles; j++) {
      for (int i=0; i<numberOfHorizontalTiles; i++) {
        KDRect tile(i*stampSize, j*stampSize, stampSize, stampSize);
        Ion::Display::pushRect(tile, (i+j)%2 == 0 ? firstStamp : secondStamp);
      }
    }
    // Check the pattern
    for (int j=0; j<numberOfVerticalTiles; j++) {
      for (int i=0; i<numberOfHorizontalTiles; i++) {
        KDRect tile((i+1)*stampSize-stampWidthPull, (j+1)*stampSize-stampHeightPull, stampWidthPull, stampHeightPull);
        Ion::Display::pullRect(tile, resultStamp);
        KDColor c = colorForStamp[(i+j)%2 == 0 ? firstColorIndex : secondColorIndex];
        for (int k = 0; k < stampWidthPull * stampHeightPull; k++) {
          if (resultStamp[k] != c) {
            numberOfInvalidPixels++;
          }
        }
      }
    }
  }
  return numberOfInvalidPixels;
}

}
}
