#ifndef POINCARE_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/bracket_layout_node.h>

namespace Poincare {

class ParenthesisLayoutNode : public BracketLayoutNode {
  friend class SequenceLayoutNode;
public:
  using BracketLayoutNode::BracketLayoutNode;
  constexpr static KDCoordinate ParenthesisWidth() { return k_widthMargin + k_lineThickness + k_externWidthMargin; }
  constexpr static KDCoordinate k_parenthesisCurveWidth = 5;
  constexpr static KDCoordinate k_parenthesisCurveHeight = 7;
  constexpr static KDCoordinate k_externWidthMargin = 1;
  constexpr static KDCoordinate k_externHeightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_verticalMargin = 4;

  // TreeNode
  size_t size() const override { return sizeof(ParenthesisLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "ParenthesisLayout"; }
#endif

protected:
  static KDColor s_parenthesisWorkingBuffer[k_parenthesisCurveHeight*k_parenthesisCurveWidth];
  void computeSize() override {
    m_frame.setSize(KDSize(ParenthesisWidth(), HeightGivenChildHeight(childHeight())));
    m_sized = true;
  }
  static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) {
    return childHeight + k_verticalMargin;
  }
  static KDCoordinate ChildHeightGivenLayoutHeight(KDCoordinate layoutHeight) {
    return layoutHeight - k_verticalMargin;
  }
};

}

#endif
