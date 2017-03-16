#ifndef SHARED_OK_VIEW_H
#define SHARED_OK_VIEW_H

#include <escher.h>

namespace Shared {

class OkView : public View {
public:
  using View::View;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_okSize = 20;
};

}

#endif

