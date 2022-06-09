#ifndef POINCARE_INTEGRAL_LAYOUT_NODE_H
#define POINCARE_INTEGRAL_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <poincare/layout_helper.h>
#include <kandinsky/font.h>

namespace Poincare {

class IntegralLayoutNode final : public LayoutNode {
public:

  // Sizes of the upper and lower curls of the integral symbol
  constexpr static KDCoordinate k_symbolHeight = 9;
  constexpr static KDCoordinate k_symbolWidth = 4;

  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::IntegralLayout; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  LayoutNode * layoutToPointWhenInserting(Expression * correspondingExpression) override { return lowerBoundLayout(); }
  Layout XNTLayout(int childIndex = -1) const override;

  // TreeNode
  size_t size() const override { return sizeof(IntegralLayoutNode); }
  int numberOfChildren() const override { return 4; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "IntegralLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDCoordinate centralArgumentHeight();
  KDPoint positionOfChild(LayoutNode * child) override;

private:
  constexpr static int k_integrandLayoutIndex = 0;
  constexpr static int k_differentialLayoutIndex = 1;
  constexpr static const KDFont * k_font = KDFont::LargeFont;
  constexpr static KDCoordinate k_boundVerticalMargin = 4;
  constexpr static KDCoordinate k_boundHorizontalMargin = 3;
  constexpr static KDCoordinate k_differentialHorizontalMargin = 3;
  constexpr static KDCoordinate k_integrandHorizontalMargin = 2;
  constexpr static KDCoordinate k_integrandVerticalMargin = 3;
  constexpr static KDCoordinate k_lineThickness = 1;
  // int(f(x), x, a, b)
  LayoutNode * integrandLayout() { return childAtIndex(k_integrandLayoutIndex); } // f(x)
  LayoutNode * differentialLayout() { return childAtIndex(k_differentialLayoutIndex); } // dx
  LayoutNode * lowerBoundLayout() { return childAtIndex(2); } // a
  LayoutNode * upperBoundLayout() { return childAtIndex(3); } // b
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;

  enum class BoundPosition : uint8_t{
    UpperBound,
    LowerBound
  };

  enum class NestedPosition : uint8_t{
    Previous,
    Next
  };

  LayoutNode * boundLayout(BoundPosition position) { return position == BoundPosition::UpperBound ? upperBoundLayout() : lowerBoundLayout(); }
  IntegralLayoutNode * nextNestedIntegral();
  IntegralLayoutNode * previousNestedIntegral();
  IntegralLayoutNode * nestedIntegral(NestedPosition position) { return position == NestedPosition::Next ? nextNestedIntegral() : previousNestedIntegral(); }
  KDCoordinate boundMaxHeight(BoundPosition position);
  IntegralLayoutNode * mostNestedIntegral (NestedPosition position);
};

class IntegralLayout final : public LayoutFourChildren<IntegralLayout, IntegralLayoutNode> {
public:
  IntegralLayout() = delete;
};

}

#endif
