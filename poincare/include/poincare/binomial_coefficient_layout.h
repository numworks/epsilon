#ifndef POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H
#define POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <poincare/grid_layout.h>

namespace Poincare {

class BinomialCoefficientLayoutNode final : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::BinomialCoefficientLayout; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(BinomialCoefficientLayoutNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "BinomialCoefficientLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  KDCoordinate knHeight() { return nLayout()->layoutSize().height() + GridLayoutNode::k_gridEntryMargin + kLayout()->layoutSize().height(); }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  LayoutNode * nLayout() { return childAtIndex(0); }
  LayoutNode * kLayout() { return childAtIndex(1); }
};

class BinomialCoefficientLayout final : public Layout {
public:
  static BinomialCoefficientLayout Builder(Layout child0, Layout child1) { return TreeHandle::FixedArityBuilder<BinomialCoefficientLayout, BinomialCoefficientLayoutNode>(ArrayBuilder<TreeHandle>(child0, child1).array(), 2); }
  BinomialCoefficientLayout() = delete;
};

}

#endif
