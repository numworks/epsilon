#ifndef ESCHER_ELLIPSIS_VIEW_H
#define ESCHER_ELLIPSIS_VIEW_H

#include <escher/transparent_view.h>

namespace Escher {

class EllipsisView : public TransparentView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  /* k_ellipsisHeight and k_ellipsisWidth are the dimensions of the ellipsis. */
  constexpr static KDCoordinate k_ellipsisHeight = 3;
  constexpr static KDCoordinate k_ellipsisWidth = 17;
};

}  // namespace Escher

#endif
