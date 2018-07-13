#ifndef POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/layout_engine.h>
#include <poincare/parenthesis_layout_node.h>

namespace Poincare {

class LeftParenthesisLayoutNode : public ParenthesisLayoutNode {
//TODO  friend class BinomialCoefficientLayout;
  friend class SequenceLayoutNode;
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, '(');
  }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isLeftParenthesis() const override { return true; }
  // TreeNode
  size_t size() const override { return sizeof(LeftParenthesisLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "Left parenthesis"; }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class LeftParenthesisLayoutRef : public LayoutReference<LeftParenthesisLayoutNode> {
public:
  LeftParenthesisLayoutRef(TreeNode * n) : LayoutReference<LeftParenthesisLayoutNode>(n) {}
  LeftParenthesisLayoutRef() : LayoutReference<LeftParenthesisLayoutNode>() {}
};

}

#endif
