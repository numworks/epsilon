#ifndef POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/parenthesis_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class RightParenthesisLayoutNode : public ParenthesisLayoutNode {
  friend class SequenceLayoutNode;
  friend class LayoutReference<RightParenthesisLayoutNode>;
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, ')');
  }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isRightParenthesis() const override { return true; }
  // TreeNode
  size_t size() const override { return sizeof(RightParenthesisLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "Right parenthesis"; }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class RightParenthesisLayoutRef : public LayoutReference<RightParenthesisLayoutNode> {
public:
  RightParenthesisLayoutRef(TreeNode * n) : LayoutReference<RightParenthesisLayoutNode>(n) {}
  RightParenthesisLayoutRef() : LayoutReference<RightParenthesisLayoutNode>() {}
};

}

#endif
