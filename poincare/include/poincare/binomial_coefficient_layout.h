#ifndef POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H
#define POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
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
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(BinomialCoefficientLayoutNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "BinomialCoefficientLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize(const KDFont * font) override;
  KDCoordinate computeBaseline(const KDFont * font) override;
  KDPoint positionOfChild(LayoutNode * child, const KDFont * font) override;
private:
  KDCoordinate knHeight(const KDFont * font) { return nLayout()->layoutSize(font).height() + GridLayoutNode::k_gridEntryMargin + kLayout()->layoutSize(font).height(); }
  void render(KDContext * ctx, KDPoint p, const KDFont * font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  LayoutNode * nLayout() { return childAtIndex(0); }
  LayoutNode * kLayout() { return childAtIndex(1); }
};

class BinomialCoefficientLayout final : public LayoutTwoChildren<BinomialCoefficientLayout, BinomialCoefficientLayoutNode> {
public:
  BinomialCoefficientLayout() = delete;
};

}

#endif
