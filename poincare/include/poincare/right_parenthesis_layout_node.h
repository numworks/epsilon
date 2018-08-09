#ifndef POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/parenthesis_layout_node.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class RightParenthesisLayoutNode : public ParenthesisLayoutNode {
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  // LayoutNode
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isRightParenthesis() const override { return true; }

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Char(buffer, bufferSize, ')');
  }

  // TreeNode
  static RightParenthesisLayoutNode * FailedAllocationStaticNode();
  RightParenthesisLayoutNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  size_t size() const override { return sizeof(RightParenthesisLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "RightParenthesisLayout"; }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class RightParenthesisLayoutRef : public LayoutReference {
public:
  RightParenthesisLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<RightParenthesisLayoutNode>()) {}
};

}

#endif
