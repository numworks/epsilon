#ifndef POINCARE_PRODUCT_LAYOUT_NODE_H
#define POINCARE_PRODUCT_LAYOUT_NODE_H

#include <poincare/layout_engine.h>
#include <poincare/sequence_layout_node.h>

namespace Poincare {

class ProductLayoutNode : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  constexpr static KDCoordinate k_lineThickness = 1;
};

class ProductLayoutRef : public LayoutReference<ProductLayoutNode> {
public:
  ProductLayoutRef(TreeNode * n) : LayoutReference<ProductLayoutNode>(n) {}
  ProductLayoutRef(LayoutRef argument, LayoutRef lowerB, LayoutRef upperB) : LayoutReference<ProductLayoutNode>() {
    addChildTreeAtIndex(argument, 0);
    addChildTreeAtIndex(lowerB, 1);
    addChildTreeAtIndex(upperB, 2);
  }
};

}

#endif
