#ifndef SHARED_MEMOIZED_CURSOR_VIEW_H
#define SHARED_MEMOIZED_CURSOR_VIEW_H

#include "cursor_view.h"

namespace Shared {

template <KDCoordinate Size>
class MemoizedCursorView : public CursorView {
public:
  MemoizedCursorView() : m_underneathPixelBufferLoaded(false) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setColor(KDColor color);
  void setCursorFrame(KDRect frame, bool force) override;
  void resetMemoization() const { m_underneathPixelBufferLoaded = false; }
  virtual void drawCursor(KDContext * ctx, KDRect rect) const = 0;
protected:
  void markRectAsDirty(KDRect rect) override;
  KDColor m_color;
  mutable KDColor m_underneathPixelBuffer[Size*Size];
  mutable bool m_underneathPixelBufferLoaded;
private:
  bool eraseCursorIfPossible();
};

}

#endif
