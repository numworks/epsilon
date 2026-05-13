#pragma once

#include <escher/theme/components.h>
#include <escher/view.h>

namespace Elements {

class SingleElementView : public Escher::View {
 public:
  SingleElementView(Escher::ThemeComponent backgroundColor)
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

  Escher::ThemeComponent m_backgroundColor;
};

}  // namespace Elements
