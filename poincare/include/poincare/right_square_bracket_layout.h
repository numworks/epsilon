#ifndef POINCARE_RIGHT_SQUARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_RIGHT_SQUARE_BRACKET_LAYOUT_NODE_H

#include <poincare/square_bracket_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class RightSquareBracketLayoutNode final : public SquareBracketLayoutNode {
public:
  using SquareBracketLayoutNode::SquareBracketLayoutNode;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Char(buffer, bufferSize, ']');
  }
  bool isRightBracket() const override { return true; }

  // TreeNode
  size_t size() const override { return sizeof(RightSquareBracketLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "RightSquareBracketLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class RightSquareBracketLayout final : public Layout {
public:
  static RightSquareBracketLayout Builder();
  RightSquareBracketLayout() = delete;
};

}

#endif
