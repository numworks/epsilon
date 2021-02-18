#ifndef ESCHER_UNEQUAL_VIEW_H
#define ESCHER_UNEQUAL_VIEW_H

#include <escher/transparent_view.h>

namespace Escher {

class UnequalView : public TransparentView {
public:
  constexpr static KDCoordinate k_size = 9;
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_size, k_size); }
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif
