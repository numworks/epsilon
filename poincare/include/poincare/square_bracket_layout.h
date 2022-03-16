#ifndef POINCARE_SQUARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_SQUARE_BRACKET_LAYOUT_NODE_H

#include "bracket_layout.h"

namespace Poincare {

class SquareBracketLayoutNode : public BracketLayoutNode {
public:
  using BracketLayoutNode::BracketLayoutNode;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_externWidthMargin = 2;
  constexpr static KDCoordinate k_squareBracketWidth = k_externWidthMargin + k_lineThickness + k_widthMargin;
protected:
  constexpr static KDCoordinate k_bracketWidth = 5;
  KDSize computeSize() override {
    return KDSize(k_squareBracketWidth, childHeight() + k_lineThickness);
  }
};
}

#endif
