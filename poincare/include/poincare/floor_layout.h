#ifndef POINCARE_FLOOR_LAYOUT_NODE_H
#define POINCARE_FLOOR_LAYOUT_NODE_H

#include <poincare/floor.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_bracket_pair_layout.h>

namespace Poincare {

class FloorLayoutNode final : public SquareBracketPairLayoutNode {
public:
  // Layout
  Type type() const override { return Type::FloorLayout; }

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Floor::s_functionHelper.aliasesList().mainAlias(), true);
  }

  // TreeNode
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "FloorLayout";
  }
#endif

protected:
  // SquareBracketPairLayoutNode
  bool renderTopBar() const override { return false; }
};

class FloorLayout final : public LayoutOneChild<FloorLayout, FloorLayoutNode> {
public:
  FloorLayout() = delete;
};

}

#endif
