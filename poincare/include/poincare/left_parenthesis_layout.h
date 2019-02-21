#ifndef POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class LeftParenthesisLayoutNode final : public ParenthesisLayoutNode {
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
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "LeftParenthesisLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class LeftParenthesisLayout final : public Layout {
public:
  static LeftParenthesisLayout Builder() { return LeftParenthesisLayout(); }
private:
  LeftParenthesisLayout() : Layout(TreePool::sharedPool()->createTreeNode<LeftParenthesisLayoutNode>()) {}
};

}

#endif
