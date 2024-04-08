#ifndef POINCARE_SUM_LAYOUT_NODE_H
#define POINCARE_SUM_LAYOUT_NODE_H

#include "layout_helper.h"
#include "sequence_layout.h"

namespace Poincare {

class SumLayoutNode final : public SequenceLayoutNode {
 public:
  using SequenceLayoutNode::SequenceLayoutNode;

  // Layout
  Type otype() const override { return Type::SumLayout; }

  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  size_t size() const override { return sizeof(SumLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "SumLayout";
  }
#endif

 private:
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;
};

class SumLayout final : public LayoutFourChildren<SumLayout, SumLayoutNode> {
 public:
  using LayoutBuilder::Builder;
  SumLayout() = delete;
};

}  // namespace Poincare

#endif
