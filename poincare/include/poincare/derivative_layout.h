#ifndef POINCARE_DERIVATIVE_LAYOUT_H
#define POINCARE_DERIVATIVE_LAYOUT_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class DerivativeLayoutNode : public LayoutNode {
public:
  DerivativeLayoutNode() : LayoutNode(), m_variableChildInFractionSlot(true) {}

  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  Layout XNTLayout(int childIndex = -1) const override;

protected:
  // diff(f(x), x, a, ..)
  constexpr static int k_derivandLayoutIndex = 0;
  constexpr static int k_variableLayoutIndex = 1;
  constexpr static int k_abscissaLayoutIndex = 2;

  LayoutNode * derivandLayout() { return childAtIndex(k_derivandLayoutIndex); }
  LayoutNode * variableLayout() { return childAtIndex(k_variableLayoutIndex); }
  LayoutNode * abscissaLayout() { return childAtIndex(k_abscissaLayoutIndex); }

  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;

  KDPoint positionOfVariableInFractionSlot(KDFont::Size font);
  KDPoint positionOfVariableInAssignmentSlot(KDFont::Size font);
  KDCoordinate abscissaBaseline(KDFont::Size font);
  KDCoordinate fractionBarWidth(KDFont::Size font);
  KDCoordinate parenthesesWidth(KDFont::Size font);

  void setVariableSlot(bool fractionSlot, bool * shouldRecomputeLayout);

  constexpr static KDCoordinate k_dxHorizontalMargin = 2;
  constexpr static KDCoordinate k_barHorizontalMargin = 2;
  constexpr static KDCoordinate k_barWidth = 1;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;

 /* There are two slots for the variable name: the Fraction and the Assignment slots.
  * This member is used to make the two copies of the variable name interactive while storing the variable name only once. */
  bool m_variableChildInFractionSlot;
};

class FirstOrderDerivativeLayoutNode final : public DerivativeLayoutNode {
  // diff(f(x), x, a)
public:
  // LayoutNode
  Type type() const override { return Type::FirstOrderDerivativeLayout; }
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;

  // TreeNode
  size_t size() const override { return sizeof(FirstOrderDerivativeLayoutNode); }
  int numberOfChildren() const override { return k_numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "FirstOrderDerivativeLayout";
  }
#endif

private:
  constexpr static int k_numberOfChildren = 3;

};

class HigherOrderDerivativeLayoutNode final : public DerivativeLayoutNode {
  // diff(f(x), x, a, n)
public:
  HigherOrderDerivativeLayoutNode() : DerivativeLayoutNode(), m_orderInDenominator(true) {}
  // LayoutNode
  Type type() const override { return Type::HigherOrderDerivativeLayout; }
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;

    // TreeNode
  size_t size() const override { return sizeof(HigherOrderDerivativeLayoutNode); }
  int numberOfChildren() const override { return k_numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "HigherOrderDerivativeLayout";
  }
#endif

private:
  constexpr static int k_numberOfChildren = 4;
  constexpr static int k_orderLayoutIndex = k_numberOfChildren - 1;
  LayoutNode * orderLayout() { return childAtIndex(k_orderLayoutIndex); }

  void setOrderSlot(bool denominator, bool * shouldRecomputeLayout);
  /* There are two slots for the order of the derivative: the numerator and
   * the denominator slots. This member is used to make the two copies of the
   * order interactive while storing the order only once. */
  bool m_orderInDenominator;
};

class FirstOrderDerivativeLayout final : public LayoutThreeChildren<FirstOrderDerivativeLayout, FirstOrderDerivativeLayoutNode> {
public:
  FirstOrderDerivativeLayout() = delete;
};

class HigherOrderDerivativeLayout final : public LayoutFourChildren<HigherOrderDerivativeLayout, HigherOrderDerivativeLayoutNode> {
public:
  HigherOrderDerivativeLayout() = delete;
};


}

#endif
