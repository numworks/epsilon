#ifndef SHARED_OK_VIEW_H
#define SHARED_OK_VIEW_H

#include <escher/transparent_view.h>

namespace Shared {

class OkView : public Escher::TransparentView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_okSize = 20;
};

}  // namespace Shared

#endif
