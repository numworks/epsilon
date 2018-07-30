#ifndef POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/parenthesis_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class RightParenthesisLayoutNode : public ParenthesisLayoutNode {
  //TODO friend class SequenceLayoutNode;
  //TODO friend class LayoutReference<RightParenthesisLayoutNode>;
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  // LayoutNode
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isRightParenthesis() const override { return true; }

  // SerializableNode
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, ')');
  }

  // TreeNode
  size_t size() const override { return sizeof(RightParenthesisLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "Right parenthesis"; }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class RightParenthesisLayoutRef : public LayoutReference {
public:
  RightParenthesisLayoutRef(TreeNode * t) : LayoutReference(t) {}
  RightParenthesisLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<RightParenthesisLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
