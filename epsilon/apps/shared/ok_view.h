#ifndef SHARED_OK_VIEW_H
#define SHARED_OK_VIEW_H

#include <escher/arbitrary_shaped_view.h>

namespace Shared {

class OkView : public Escher::ArbitraryShapedView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_okSize = 20;
};

}  // namespace Shared

#endif
