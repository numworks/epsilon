#ifndef POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H
#define POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>
#include <poincare/grid_layout_node.h>

namespace Poincare {

class BinomialCoefficientLayoutNode : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // SerializableNode
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(BinomialCoefficientLayoutNode); }
#if TREE_LOG
  const char * description() const override {
    return "BinomialCoefficientLayout";
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  KDCoordinate knHeight() { return nLayout()->layoutSize().height() + GridLayoutNode::k_gridEntryMargin + kLayout()->layoutSize().height(); }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  LayoutNode * nLayout() {
    assert(numberOfChildren() == 2);
    return childAtIndex(0);
  }
  LayoutNode * kLayout() {
    assert(numberOfChildren() == 2);
    return childAtIndex(1);
  }
};

class BinomialCoefficientLayoutRef : public LayoutReference<BinomialCoefficientLayoutNode> {
public:
  BinomialCoefficientLayoutRef(LayoutRef n, LayoutRef k) :
    LayoutReference<BinomialCoefficientLayoutNode>()
  {
    addChildTreeAtIndex(n, 0);
    addChildTreeAtIndex(k, 1);
  }
  BinomialCoefficientLayoutRef(TreeNode * t) : LayoutReference<BinomialCoefficientLayoutNode>(t) {}
};

}

#endif
