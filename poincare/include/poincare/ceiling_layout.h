#ifndef POINCARE_CEILING_LAYOUT_NODE_H
#define POINCARE_CEILING_LAYOUT_NODE_H

#include <poincare/ceiling.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class CeilingLayoutNode final : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;

  // Layout
  Type type() const override { return Type::CeilingLayout; }

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Ceiling::s_functionHelper.name(), true);
  }

  // TreeNode
  size_t size() const override { return sizeof(CeilingLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "CeilingLayout";
  }
#endif

protected:
  bool renderBottomBar() const override { return false; }
};

class CeilingLayout final : public LayoutOneChild<CeilingLayout, CeilingLayoutNode> {
public:
  CeilingLayout() = delete;
};

}

#endif
