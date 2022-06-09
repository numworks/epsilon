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
  Type type() const override { return Type::DerivativeLayout; }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  Layout XNTLayout(int childIndex = -1) const override;

  // TreeNode
  size_t size() const override { return sizeof(DerivativeLayoutNode); }
  int numberOfChildren() const override { return 3; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "DerivativeLayout";
  }
#endif

protected:
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;

private:
  // diff(f(x), x, a)
  static constexpr int k_derivandLayoutIndex = 0;
  static constexpr int k_variableLayoutIndex = 1;
  static constexpr int k_abscissaLayoutIndex = 2;
  LayoutNode * derivandLayout() { return childAtIndex(k_derivandLayoutIndex); }
  LayoutNode * variableLayout() { return childAtIndex(k_variableLayoutIndex); }
  LayoutNode * abscissaLayout() { return childAtIndex(k_abscissaLayoutIndex); }

  KDPoint positionOfVariableInFractionSlot();
  KDPoint positionOfVariableInAssignmentSlot();
  KDCoordinate abscissaBaseline();
  KDCoordinate fractionBarWidth();
  KDCoordinate parenthesesWidth();

  void setVariableSlot(bool fractionSlot, bool * shouldRecomputeLayout);

  static constexpr KDCoordinate k_dxHorizontalMargin = 2;
  static constexpr KDCoordinate k_barHorizontalMargin = 2;
  static constexpr KDCoordinate k_barWidth = 1;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;

 /* There are two slots for the variable name: the Fraction and the Assignment slots.
  * This member is used to make the two copies of the variable name interactive while storing the variable name only once. */
  bool m_variableChildInFractionSlot;
};

class DerivativeLayout final : public LayoutThreeChildren<DerivativeLayout, DerivativeLayoutNode> {
public:
  DerivativeLayout() = delete;
};

}

#endif
