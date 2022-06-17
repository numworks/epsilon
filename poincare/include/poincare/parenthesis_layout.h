#ifndef POINCARE_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/bracket_layout.h>
#include <escher/metric.h>
#include <algorithm>

namespace Poincare {

class ParenthesisLayoutNode : public BracketLayoutNode {
  friend class SequenceLayoutNode;
public:
  using BracketLayoutNode::BracketLayoutNode;
  constexpr static KDCoordinate k_parenthesisCurveWidth = 5;
  constexpr static KDCoordinate k_parenthesisCurveHeight = 7;
  constexpr static KDCoordinate k_externWidthMargin = 1;
  constexpr static KDCoordinate k_externHeightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_verticalMargin = 4;
  constexpr static KDCoordinate k_parenthesisWidth = k_widthMargin + k_lineThickness + k_externWidthMargin;

  // TreeNode
  size_t size() const override { return sizeof(ParenthesisLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ParenthesisLayout";
  }
#endif

protected:
  KDSize computeSize() override {
    return KDSize(k_parenthesisWidth, HeightGivenChildHeight(childHeight()));
  }
  static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) {
    return std::max<KDCoordinate>(childHeight, Escher::Metric::MinimalBracketAndParenthesisHeight) + k_verticalMargin;
  }
  static KDCoordinate ChildHeightGivenLayoutHeight(KDCoordinate layoutHeight) {
    return layoutHeight - k_verticalMargin;
  }
};

}

#endif
