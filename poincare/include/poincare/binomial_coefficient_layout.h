#ifndef POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H
#define POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
//#include <poincare/grid_layout.h>

namespace Poincare {

class BinomialCoefficientLayoutNode final : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::BinomialCoefficientLayout; }

  int indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex) const override;
  int indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex) const override;

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
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;
private:
  KDCoordinate knHeight(KDFont::Size font) { return nLayout()->layoutSize(font).height() + /*TODO: GridLayoutNode::k_gridEntryMargin*/6 + kLayout()->layoutSize(font).height(); }
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;

  constexpr static int k_nLayoutIndex = 0;
  constexpr static int k_kLayoutIndex = 1;
  LayoutNode * nLayout() { return childAtIndex(k_nLayoutIndex); }
  LayoutNode * kLayout() { return childAtIndex(k_kLayoutIndex); }
};

class BinomialCoefficientLayout final : public LayoutTwoChildren<BinomialCoefficientLayout, BinomialCoefficientLayoutNode> {
public:
  BinomialCoefficientLayout() = delete;
};

}

#endif
