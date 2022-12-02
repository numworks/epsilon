#ifndef SHARED_MEMOIZED_CURSOR_VIEW_H
#define SHARED_MEMOIZED_CURSOR_VIEW_H

#include "cursor_view.h"
#include <escher/palette.h>

namespace Shared {

class MemoizedCursorView : public CursorView {
public:
  MemoizedCursorView() : m_underneathPixelBufferLoaded(false) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setColor(KDColor color) override;
  void setCursorFrame(KDRect frame, bool force) override;
  void resetMemoization() const { m_underneathPixelBufferLoaded = false; }
  void redrawCursor(KDRect rect);
protected:
  virtual void drawCursor(KDContext * ctx, KDRect rect) const = 0;
  virtual KDCoordinate size() const = 0;
  virtual KDColor * underneathPixelBuffer() const = 0;
  void markRectAsDirty(KDRect rect) override;
  KDColor color() const { return isHighlighted() ? Escher::Palette::GrayDarkest : m_color; }
  mutable bool m_underneathPixelBufferLoaded;
private:
  KDColor m_color;
  bool eraseCursorIfPossible();
};

}

#endif
