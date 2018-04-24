#ifndef POINCARE_SQUARE_BRACKET_LAYOUT_H
#define POINCARE_SQUARE_BRACKET_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class SquareBracketLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
protected:
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_externWidthMargin = 2;
  KDSize computeSize() override {
    return KDSize(k_externWidthMargin + k_lineThickness + k_widthMargin, operandHeight() + k_lineThickness);
  }
};
}

#endif
