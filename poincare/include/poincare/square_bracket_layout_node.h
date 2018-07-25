#ifndef POINCARE_SQUARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_SQUARE_BRACKET_LAYOUT_NODE_H

#include "bracket_layout_node.h"

namespace Poincare {

class SquareBracketLayoutNode : public BracketLayoutNode {
public:
  using BracketLayoutNode::BracketLayoutNode;
  constexpr static KDCoordinate k_lineThickness = 1;
  KDCoordinate static BracketWidth() { return k_externWidthMargin + k_lineThickness + k_widthMargin; }
protected:
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_externWidthMargin = 2;
  void computeSize() override {
    m_frame.setSize(KDSize(BracketWidth(), childHeight() + k_lineThickness));
    m_sized = true;
  }
};
}

#endif
