#ifndef POINCARE_INTEGRAL_LAYOUT_NODE_H
#define POINCARE_INTEGRAL_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>
#include <kandinsky/text.h>

namespace Poincare {

class IntegralLayoutNode : public LayoutNode {
public:
  constexpr static KDCoordinate k_symbolHeight = 4;
  constexpr static KDCoordinate k_symbolWidth = 4;

  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  LayoutNode * layoutToPointWhenInserting() override {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout();
  }
  char XNTChar() const override { return 'x'; }

  // TreeNode
  size_t size() const override { return sizeof(IntegralLayoutNode); }
  int numberOfChildren() const override { return 3; }
#if TREE_LOG
  const char * description() const override { return "IntegralLayout"; }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static KDText::FontSize k_fontSize = KDText::FontSize::Large;
  constexpr static KDCoordinate k_boundHeightMargin = 8;
  constexpr static KDCoordinate k_boundWidthMargin = 5;
  constexpr static KDCoordinate k_integrandWidthMargin = 2;
  constexpr static KDCoordinate k_integrandHeigthMargin = 2;
  constexpr static KDCoordinate k_lineThickness = 1;
  LayoutNode * integrandLayout() { return childAtIndex(0); }
  LayoutNode * lowerBoundLayout() { return childAtIndex(1); }
  LayoutNode * upperBoundLayout() { return childAtIndex(2); }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class IntegralLayoutRef : public LayoutReference {
public:
  IntegralLayoutRef(LayoutRef integrand, LayoutRef lowerBound, LayoutRef upperBound) :
    LayoutReference(TreePool::sharedPool()->createTreeNode<IntegralLayoutNode>(), true)
  {
    replaceChildAtIndexInPlace(0, integrand);
    replaceChildAtIndexInPlace(1, lowerBound);
    replaceChildAtIndexInPlace(2, upperBound);
  }
};

}

#endif
