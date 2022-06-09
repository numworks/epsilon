#ifndef POINCARE_LEFT_SQUARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_LEFT_SQUARE_BRACKET_LAYOUT_NODE_H

#include <poincare/square_bracket_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class LeftSquareBracketLayoutNode final : public SquareBracketLayoutNode {
public:
  using SquareBracketLayoutNode::SquareBracketLayoutNode;

  // Layout
  Type type() const override { return Type::LeftSquareBracketLayout; }

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::CodePoint(buffer, bufferSize, '[');
  }

  // TreeNode
  size_t size() const override { return sizeof(LeftSquareBracketLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "LeftSquareBracketLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class LeftSquareBracketLayout final : public LayoutNoChildren<LeftSquareBracketLayout, LeftSquareBracketLayoutNode> {
public:
  LeftSquareBracketLayout() = delete;
};

}

#endif
