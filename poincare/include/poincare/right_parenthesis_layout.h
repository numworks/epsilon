#ifndef POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_RIGHT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/parenthesis_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class RightParenthesisLayoutNode final : public ParenthesisLayoutNode {
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  // Layout
  Type type() const override { return Type::RightParenthesisLayout; }

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::CodePoint(buffer, bufferSize, ')');
  }

  // TreeNode
  size_t size() const override { return sizeof(RightParenthesisLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "RightParenthesisLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class RightParenthesisLayout final : public LayoutNoChildren<RightParenthesisLayout, RightParenthesisLayoutNode> {
public:
  RightParenthesisLayout() = delete;
};

}

#endif
