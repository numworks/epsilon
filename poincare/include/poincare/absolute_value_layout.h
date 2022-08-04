#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/absolute_value.h>
#include <poincare/square_bracket_pair_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class AbsoluteValueLayoutNode final : public SquareBracketPairLayoutNode {
public:
  // LayoutNode
  Type type() const override { return Type::AbsoluteValueLayout; }

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, AbsoluteValue::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::TypeOfParenthesis::System);
  }

  // TreeNode
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "AbsoluteValueLayout";
  }
#endif

private:
  constexpr static KDCoordinate k_innerWidthMargin = 2;

  // SquareBracketPairLayoutNode
  KDCoordinate bracketWidth() const override { return k_lineThickness + k_innerWidthMargin + k_externalWidthMargin; }
  KDCoordinate verticalMargin() const override { return 0; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
};

class AbsoluteValueLayout final : public LayoutOneChild<AbsoluteValueLayout,AbsoluteValueLayoutNode> {
public:
  AbsoluteValueLayout() = delete;
};

}

#endif
