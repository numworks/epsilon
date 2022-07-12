#ifndef PERIODIC_SINGLE_ELEMENT_VIEW_H
#define PERIODIC_SINGLE_ELEMENT_VIEW_H

#include "elements_view_data_source.h"
#include <escher/view.h>

namespace Periodic {

class SingleElementView : public Escher::View {
public:
  SingleElementView(KDColor backgroundColor) : m_backgroundColor(backgroundColor) {}

  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_totalSize, k_totalSize); }

private:
  constexpr static KDCoordinate k_cellSize = 46;
  constexpr static KDCoordinate k_borderSize = 2;
  constexpr static KDCoordinate k_totalSize = k_cellSize + 2 * k_borderSize;
  constexpr static KDCoordinate k_symbolZAMargin = 1;
  constexpr static KDCoordinate k_ZVerticalOffset = 6;
  constexpr static KDCoordinate k_AVerticalOffset = 9;

  KDColor m_backgroundColor;
};

}

#endif
