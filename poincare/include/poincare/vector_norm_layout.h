#ifndef POINCARE_VECTOR_NORM_LAYOUT_NODE_H
#define POINCARE_VECTOR_NORM_LAYOUT_NODE_H

#include <poincare/vector_norm.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class VectorNormLayoutNode final : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;

  // Layout
  Type type() const override { return Type::VectorNormLayout; }

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorNorm::s_functionHelper.name(), true);
  }

  // TreeNode
  size_t size() const override { return sizeof(VectorNormLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "VectorNormLayout";
  }
#endif

private:
  KDCoordinate widthMargin() const override { return 2; }
  KDCoordinate verticalMargin() const override { return 0; }
  KDCoordinate verticalExternMargin() const override { return 1; }
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
