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
  Type otype() const override { return Type::FloorLayout; }

  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(
        this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
        Floor::s_functionHelper.aliasesList().mainAlias(),
        SerializationHelper::ParenthesisType::System);
  }

  // PoolObject
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
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

}  // namespace Poincare

#endif
