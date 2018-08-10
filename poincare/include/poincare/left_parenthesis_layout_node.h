#ifndef POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/parenthesis_layout_node.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class LeftParenthesisLayoutNode : public ParenthesisLayoutNode {
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  // Layout Node
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isLeftParenthesis() const override { return true; }

  // Serializable Node
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Char(buffer, bufferSize, '(');
  }

  // TreeNode
  static LeftParenthesisLayoutNode * FailedAllocationStaticNode();
  LeftParenthesisLayoutNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const {
    stream << "LeftParenthesisLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class LeftParenthesisLayoutRef : public LayoutReference {
public:
  LeftParenthesisLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<LeftParenthesisLayoutNode>()) {}
};

}

#endif
