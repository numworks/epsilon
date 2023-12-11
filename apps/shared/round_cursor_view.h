#ifndef SHARED_ROUND_CURSOR_VIEW_H
#define SHARED_ROUND_CURSOR_VIEW_H

#include "dots.h"
#include "memoized_cursor_view.h"

namespace Shared {

class AbstractRoundCursorView : public MemoizedCursorView {
 public:
  AbstractRoundCursorView(bool isRing)
      : MemoizedCursorView(), m_isRing(isRing) {}

 protected:
  bool m_isRing;
  constexpr static size_t k_maxSize =
      std::max(Dots::LargeDotDiameter, Dots::LargeRingDiameter);

 private:
  void drawCursor(KDContext* ctx, KDRect rect) const override;
  KDColor* underneathPixelBuffer() const override {
    return m_underneathPixelBuffer;
  }
  mutable KDColor m_underneathPixelBuffer[k_maxSize * k_maxSize];
};

class RoundCursorView : public AbstractRoundCursorView {
 public:
  RoundCursorView() : AbstractRoundCursorView(false) {}

 private:
  KDCoordinate size() const override { return Dots::LargeDotDiameter; }
};

class RingCursorView : public AbstractRoundCursorView {
 public:
  RingCursorView() : AbstractRoundCursorView(true) {}

 private:
  KDCoordinate size() const override { return Dots::LargeRingDiameter; }
};

class ToggleableRingRoundCursorView : public AbstractRoundCursorView {
 public:
  ToggleableRingRoundCursorView() : AbstractRoundCursorView(false) {}
  void setIsRing(bool isRing);

 private:
  KDCoordinate size() const override { return k_maxSize; }
};

}  // namespace Shared

#endif
