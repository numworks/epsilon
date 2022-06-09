#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/absolute_value.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class AbsoluteValueLayoutNode final : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;

  // Layout
  Type type() const override { return Type::AbsoluteValueLayout; }

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, AbsoluteValue::s_functionHelper.name(), true);
  }

  // TreeNode
  size_t size() const override { return sizeof(AbsoluteValueLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "AbsoluteValueLayout";
  }
#endif

private:
  KDCoordinate widthMargin() const override { return 2; }
  KDCoordinate verticalMargin() const override { return 0; }
  KDCoordinate verticalExternMargin() const override { return 1; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
};

class AbsoluteValueLayout final : public LayoutOneChild<AbsoluteValueLayout,AbsoluteValueLayoutNode> {
public:
  AbsoluteValueLayout() = delete;
};

}

#endif
