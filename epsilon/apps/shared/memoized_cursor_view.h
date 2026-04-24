#pragma once

#include <escher/theme/components.h>

#include "cursor_view.h"

namespace Shared {

class MemoizedCursorView : public CursorView {
 public:
  MemoizedCursorView() : m_underneathPixelBufferLoaded(false) {}
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setColor(Escher::ThemeComponent color,
                const Escher::View* parent) override;
  void setCursorFrame(View* parent, KDRect frame, bool force) override;
  void resetMemoization() const { m_underneathPixelBufferLoaded = false; }
  void redrawCursor(KDRect rect);

 protected:
  virtual void drawCursor(KDContext* ctx, KDRect rect) const = 0;
  virtual KDCoordinate size() const = 0;
  virtual KDColor* underneathPixelBuffer() const = 0;
  Escher::ThemeComponent color() const {
    return isHighlighted() ? Escher::ThemeComponent::TokenGrayDarkest : m_color;
  }
  mutable bool m_underneathPixelBufferLoaded;
  bool eraseCursorIfPossible(const Escher::View* parent);

 private:
  Escher::ThemeComponent m_color = Escher::ThemeComponent::DataRed;
};

}  // namespace Shared
