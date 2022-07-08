#ifndef PERIODIC_SINGLE_ELEMENT_VIEW_H
#define PERIODIC_SINGLE_ELEMENT_VIEW_H

#include "elements_view_data_source.h"
#include <escher/view.h>

namespace Periodic {

class SingleElementView : public Escher::View {
public:
  void drawRect(KDContext * ctx, KDRect rect) const override;

private:
  constexpr static KDColor k_backgroundColor = KDColorWhite;
  constexpr static KDCoordinate k_cellSize = 48;
  constexpr static KDCoordinate k_borderSize = 2;
  constexpr static KDCoordinate k_outerMargin = 11;
  constexpr static KDCoordinate k_symbolZAMargin = 1;
  constexpr static KDCoordinate k_ZVerticalOffset = 6;
  constexpr static KDCoordinate k_AVerticalOffset = 9;
  constexpr static KDCoordinate k_nameVerticalOffset = 2;
};

}

#endif
