#ifndef ESCHER_CHEVRON_VIEW_H
#define ESCHER_CHEVRON_VIEW_H

#include <escher/transparent_view.h>

namespace Escher {

class ChevronView : public TransparentView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  /* k_chevronHeight and k_chevronWidth are the dimensions of the chevron. */
  constexpr static KDCoordinate k_chevronHeight = 10;
  constexpr static KDCoordinate k_chevronWidth = 8;
};

}  // namespace Escher

#endif
