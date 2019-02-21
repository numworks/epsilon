#ifndef POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/parenthesis_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class RightParenthesisLayoutNode final : public ParenthesisLayoutNode {
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
  size_t size() const override { return sizeof(RightParenthesisLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "RightParenthesisLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class RightParenthesisLayout final : public Layout {
public:
  static RightParenthesisLayout Builder();
  RightParenthesisLayout() = delete;
};

}

#endif
