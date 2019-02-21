#ifndef POINCARE_PRODUCT_LAYOUT_NODE_H
#define POINCARE_PRODUCT_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/sequence_layout.h>

namespace Poincare {

class ProductLayoutNode final : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  size_t size() const override { return sizeof(ProductLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ProductLayout";
  }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  constexpr static KDCoordinate k_lineThickness = 1;
};

class ProductLayout final : public Layout {
public:
  static ProductLayout Builder(Layout argument, Layout variable, Layout lowerB, Layout upperB)  { return TreeHandle::FixedArityBuilder<ProductLayout,ProductLayoutNode>(ArrayBuilder<TreeHandle>(argument, variable, lowerB, upperB).array(), 4); }
  ProductLayout() = delete;
};

}

#endif
