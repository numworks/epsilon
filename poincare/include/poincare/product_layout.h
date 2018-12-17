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
  ProductLayout(Layout argument, Layout variable, Layout lowerB, Layout upperB) :
    Layout(TreePool::sharedPool()->createTreeNode<ProductLayoutNode>())
  {
    replaceChildAtIndexInPlace(0, argument);
    replaceChildAtIndexInPlace(1, variable);
    replaceChildAtIndexInPlace(2, lowerB);
    replaceChildAtIndexInPlace(3, upperB);
  }
};

}

#endif
