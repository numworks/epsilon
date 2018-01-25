#ifndef SHARED_ROUND_CURSOR_VIEW_H
#define SHARED_ROUND_CURSOR_VIEW_H

#include <escher.h>

namespace Shared {

class RoundCursorView : public View {
public:
  RoundCursorView(KDColor color = KDColorBlack);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setColor(KDColor color);
private:
  KDColor m_color;
};

}

#endif
