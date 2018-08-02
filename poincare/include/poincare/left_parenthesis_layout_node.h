#ifndef POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/layout_engine.h>
#include <poincare/parenthesis_layout_node.h>

namespace Poincare {

class LeftParenthesisLayoutNode : public ParenthesisLayoutNode {
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  // Layout Node
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isLeftParenthesis() const override { return true; }

  // Serializable Node
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
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

class LeftParenthesisLayoutRef : public LayoutReference {
public:
  LeftParenthesisLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<LeftParenthesisLayoutNode>();
    m_identifier = node->identifier();
  }
  LeftParenthesisLayoutRef(TreeNode * n) : LayoutReference(n) {}
};

}

#endif
