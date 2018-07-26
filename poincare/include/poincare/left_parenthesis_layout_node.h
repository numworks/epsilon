#ifndef POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/layout_engine.h>
#include <poincare/parenthesis_layout_node.h>

namespace Poincare {

class LeftParenthesisLayoutNode : public ParenthesisLayoutNode {
  //TODO friend class SequenceLayoutNode;
  //TODO friend class LayoutReference<LeftParenthesisLayoutNode>;
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  // Layout Node
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isLeftParenthesis() const override { return true; }

  // Serializable Node
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, '(');
  }

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
