#ifndef ELEMENTS_SINGLE_ELEMENT_VIEW_H
#define ELEMENTS_SINGLE_ELEMENT_VIEW_H

#include <escher/view.h>

#include "elements_view_data_source.h"

namespace Elements {

class SingleElementView : public Escher::View {
 public:
  SingleElementView(KDColor backgroundColor)
      : m_backgroundColor(backgroundColor) {}

  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(k_totalSize, k_totalSize);
  }

 private:
  constexpr static KDCoordinate k_cellSize = 46;
  constexpr static KDCoordinate k_borderSize = 2;
  constexpr static KDCoordinate k_totalSize = k_cellSize + 2 * k_borderSize;
  constexpr static KDCoordinate k_symbolZAMargin = 1;
  constexpr static KDCoordinate k_ZVerticalOffset = 6;
  constexpr static KDCoordinate k_AVerticalOffset = 9;
  constexpr static KDFont::Size k_symbolFont = KDFont::Size::Large;
  constexpr static KDFont::Size k_numbersFont = KDFont::Size::Small;

  KDColor m_backgroundColor;
};

}  // namespace Elements

#endif
