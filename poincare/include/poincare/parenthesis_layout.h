#ifndef POINCARE_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/bracket_layout.h>
#include <algorithm>

namespace Poincare {

class ParenthesisLayoutNode : public BracketLayoutNode {
  friend class SequenceLayoutNode;
public:
  // Dimensions
  constexpr static KDCoordinate k_curveWidth = 5;
  constexpr static KDCoordinate k_curveHeight = 7;
  // Margins
  constexpr static KDCoordinate k_widthMargin = 1;
  constexpr static KDCoordinate k_parenthesisWidth = k_widthMargin + k_curveWidth + k_widthMargin;

  using BracketLayoutNode::BracketLayoutNode;
  // TreeNode
  size_t size() const override { return sizeof(ParenthesisLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ParenthesisLayout";
  }
#endif

private:
  KDCoordinate width() const override { return k_parenthesisWidth; }
};

}

#endif
