#ifndef POINCARE_INTEGRAL_LAYOUT_NODE_H
#define POINCARE_INTEGRAL_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <kandinsky/font.h>

namespace Poincare {

class IntegralLayoutNode final : public LayoutNode {
public:
  constexpr static KDCoordinate k_symbolHeight = 4;
  constexpr static KDCoordinate k_symbolWidth = 4;

  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::IntegralLayout; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  LayoutNode * layoutToPointWhenInserting() override { return lowerBoundLayout(); }
  CodePoint XNTCodePoint() const override { return 'x'; }

  // TreeNode
  size_t size() const override { return sizeof(IntegralLayoutNode); }
  int numberOfChildren() const override { return 4; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "IntegralLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static const KDFont * k_font = KDFont::LargeFont;
  constexpr static KDCoordinate k_boundHeightMargin = 8;
  constexpr static KDCoordinate k_boundWidthMargin = 5;
  constexpr static KDCoordinate k_differentialWidthMargin = 3;
  constexpr static KDCoordinate k_integrandWidthMargin = 2;
  constexpr static KDCoordinate k_integrandHeigthMargin = 2;
  constexpr static KDCoordinate k_lineThickness = 1;
  // int(f(x), x, a, b)
  LayoutNode * integrandLayout() { return childAtIndex(0); } // f(x)
  LayoutNode * differentialLayout() { return childAtIndex(1); } // dx
  LayoutNode * lowerBoundLayout() { return childAtIndex(2); } // a
  LayoutNode * upperBoundLayout() { return childAtIndex(3); } // b
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class IntegralLayout final : public Layout {
public:
  static IntegralLayout Builder(Layout integrand, Layout differential, Layout lowerBound, Layout upperBound) { return TreeHandle::FixedArityBuilder<IntegralLayout, IntegralLayoutNode>(ArrayBuilder<TreeHandle>(integrand, differential, lowerBound, upperBound).array(), 4); }
  IntegralLayout() = delete;
};

}

#endif
