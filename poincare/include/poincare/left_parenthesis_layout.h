#ifndef POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_LEFT_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class LeftParenthesisLayoutNode final : public ParenthesisLayoutNode {
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  // Layout
  Type type() const override { return Type::LeftParenthesisLayout; }

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  // Layout Node
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;

  // Serializable Node
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::CodePoint(buffer, bufferSize, '(');
  }

  // TreeNode
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "LeftParenthesisLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class LeftParenthesisLayout final : public Layout {
public:
  static LeftParenthesisLayout Builder() { return TreeHandle::FixedArityBuilder<LeftParenthesisLayout, LeftParenthesisLayoutNode>(); }
  LeftParenthesisLayout() = delete;
};

}

#endif
