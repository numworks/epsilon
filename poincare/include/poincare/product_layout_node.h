#ifndef POINCARE_PRODUCT_LAYOUT_NODE_H
#define POINCARE_PRODUCT_LAYOUT_NODE_H

#include <poincare/layout_engine.h>
#include <poincare/sequence_layout_node.h>

namespace Poincare {

class ProductLayoutNode : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  size_t size() const override { return sizeof(ProductLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "ProductLayout"; }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  constexpr static KDCoordinate k_lineThickness = 1;
};

class ProductLayoutRef : public LayoutReference {
public:
  ProductLayoutRef(TreeNode * n) : LayoutReference(n) {}
  ProductLayoutRef(LayoutRef argument, LayoutRef lowerB, LayoutRef upperB) :
    ProductLayoutRef()
  {
    addChildAtIndex(argument, 0, 0);
    addChildAtIndex(lowerB, 1, 1);
    addChildAtIndex(upperB, 2, 2);
  }
private:
  ProductLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<ProductLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
