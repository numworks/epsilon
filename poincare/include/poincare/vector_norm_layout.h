#ifndef POINCARE_VECTOR_NORM_LAYOUT_NODE_H
#define POINCARE_VECTOR_NORM_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_bracket_pair_layout.h>
#include <poincare/vector_norm.h>

namespace Poincare {

class VectorNormLayoutNode final : public SquareBracketPairLayoutNode {
public:
  // Layout
  Type type() const override { return Type::VectorNormLayout; }

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorNorm::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System);
  }

  // TreeNode
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "VectorNormLayout";
  }
#endif

private:
  constexpr static KDCoordinate k_innerWidthMargin = 2;

  // SquareBracketPairLayoutNode
  KDCoordinate bracketWidth() const override { return 2 * k_lineThickness + k_doubleBarMargin + k_innerWidthMargin + k_externalWidthMargin; }
  KDCoordinate verticalMargin() const override { return 0; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
  bool renderDoubleBar() const override { return true; }
};

class VectorNormLayout final : public LayoutOneChild<VectorNormLayout, VectorNormLayoutNode> {
public:
  VectorNormLayout() = delete;
};

}

#endif
