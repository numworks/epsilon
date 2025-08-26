#pragma once

/* ION abstracts pushing pixels to the screen.
 *
 * There could be a single entry point, set_pixel, but setting pixels one by one
 * incurs quite a large overhead because you need to send the coordinate of each
 * pixel to the screen.
 *
 * Many displays support sending contiguous pixels without having to repeat the
 * pixel coordinates every time. We're therefore leveraging this capability
 * which results in a very consequent speedup (up to ~10x faster). */

#include <ion/display_constants.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>
#include <omg/global_box.h>
#include <stdint.h>

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor* pixels);
void pushRectUniform(KDRect r, KDColor c);
void pullRect(KDRect r, KDColor* pixels);

bool waitForVBlank();
void refreshDisplay();

struct ScreenParameters {
  static constexpr int nbParams = 7;
  union {
    struct {
      uint8_t vop;
      uint8_t lightGray;
      uint8_t darkGray;
      uint8_t frameRate;
      uint8_t duty;
      uint8_t bias;
      uint8_t halve;
    };
    uint8_t params[nbParams];
  };
};

void reconfigure(ScreenParameters p);

class Context : public KDContext {
  friend OMG::GlobalBox<Context>;

 public:
  static OMG::GlobalBox<Context> SharedContext;
  static void Putchar(char c);
  static void Clear(KDPoint newCursorPosition = KDPointZero);

 private:
  Context();
  void pushRect(KDRect rect, const KDColor* pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor* pixels) override;
};

// For Power On Self tests
int displayUniformTilingSize10(KDColor c);
int displayColoredTilingSize10();
void POSTPushMulticolor(int rootNumberTiles, int tileSize);

void setScreenshotCallback(void (*)(void));

}  // namespace Display
}  // namespace Ion
