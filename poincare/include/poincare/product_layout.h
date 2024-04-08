#ifndef POINCARE_PRODUCT_LAYOUT_NODE_H
#define POINCARE_PRODUCT_LAYOUT_NODE_H

#include "layout_helper.h"
#include "sequence_layout.h"

namespace Poincare {

class ProductLayoutNode final : public SequenceLayoutNode {
 public:
  using SequenceLayoutNode::SequenceLayoutNode;

  // Layout
  Type otype() const override { return Type::ProductLayout; }

  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  size_t size() const override { return sizeof(ProductLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ProductLayout";
  }
#endif

 protected:
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;

 private:
  constexpr static KDCoordinate k_lineThickness = 1;
};

class ProductLayout final
    : public LayoutFourChildren<ProductLayout, ProductLayoutNode> {
 public:
  ProductLayout() = delete;
};

}  // namespace Poincare

#endif
