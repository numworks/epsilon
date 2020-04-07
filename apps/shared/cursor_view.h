#ifndef SHARED_CURSOR_VIEW_H
#define SHARED_CURSOR_VIEW_H

#include <escher/transparent_view.h>

namespace Shared {

class CursorView : public TransparentView {
public:
  virtual void setCursorFrame(KDRect frame, bool force) { View::setFrame(frame, force); }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  constexpr static KDCoordinate k_size = 19;
};

}

#endif
