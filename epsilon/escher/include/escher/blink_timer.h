#pragma once

#include <escher/text_cursor_view.h>
#include <escher/timer.h>
#include <kandinsky/point.h>

namespace Escher {

class BlinkTimer : public Timer {
 public:
  constexpr static uint32_t k_blinkPeriod = 600;
  static_assert(k_blinkPeriod > TickDuration,
                "BlinkTimer period is shorter than one tick.");

  BlinkTimer() : Timer(k_blinkPeriod / TickDuration) {}

  void forceCursorVisible();
  bool isActive() const;
  KDPoint getOrigin() const;

 private:
  static TextCursorView* SharedCursor() {
    return TextCursorView::sharedTextCursor;
  }

  bool fire() override;
};

}  // namespace Escher
