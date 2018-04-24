#ifndef POINCARE_PARENTHESIS_LAYOUT_H
#define POINCARE_PARENTHESIS_LAYOUT_H

#include "bracket_layout.h"

namespace Poincare {

class ParenthesisLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
  constexpr static KDCoordinate parenthesisWidth() { return k_widthMargin + k_lineThickness + k_externWidthMargin; }
  constexpr static KDCoordinate k_parenthesisCurveWidth = 5;
  constexpr static KDCoordinate k_parenthesisCurveHeight = 7;
  constexpr static KDCoordinate k_externWidthMargin = 1;
  constexpr static KDCoordinate k_externHeightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_verticalMargin = 4;
protected:
  KDColor s_parenthesisWorkingBuffer[k_parenthesisCurveHeight*k_parenthesisCurveWidth];
  KDSize computeSize() override {
    return KDSize(parenthesisWidth(), operandHeight() + k_verticalMargin);
  }
};

}

#endif
