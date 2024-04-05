#ifndef POINCARE_CEILING_LAYOUT_NODE_H
#define POINCARE_CEILING_LAYOUT_NODE_H

#include <poincare/ceiling.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_bracket_pair_layout.h>

namespace Poincare {

class CeilingLayoutNode final : public SquareBracketPairLayoutNode {
 public:
  // Layout
  Type otype() const override { return Type::CeilingLayout; }

  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(
        this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
        Ceiling::s_functionHelper.aliasesList().mainAlias(),
        SerializationHelper::ParenthesisType::System);
  }

  // PoolObject
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "CeilingLayout";
  }
#endif

 protected:
  // SquareBracketPairLayoutNode
  bool renderBottomBar() const override { return false; }
};

class CeilingLayout final
    : public LayoutOneChild<CeilingLayout, CeilingLayoutNode> {
 public:
  CeilingLayout() = delete;
};

}  // namespace Poincare

#endif
