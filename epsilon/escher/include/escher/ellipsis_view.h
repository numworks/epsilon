#pragma once

#include <escher/arbitrary_shaped_view.h>
#include <escher/theme/components.h>

namespace Escher {

class EllipsisView : public ArbitraryShapedView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  /* k_ellipsisHeight and k_ellipsisWidth are the dimensions of the ellipsis. */
  constexpr static KDCoordinate k_ellipsisHeight = 3;
  constexpr static KDCoordinate k_ellipsisWidth = 17;

 private:
  constexpr static ThemeComponent k_color = ThemeComponent::TokenEllipsis;
};

}  // namespace Escher
