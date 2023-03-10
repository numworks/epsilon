#ifndef POINCARE_DERIVATIVE_LAYOUT_H
#define POINCARE_DERIVATIVE_LAYOUT_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>
#include <poincare/vertical_offset_layout.h>

namespace Poincare {

class DerivativeLayoutNode : public LayoutNode {
 public:
  enum class VariableSlot : bool { Fraction, Assignment };

  // diff(f(x), x, a, ..)
  constexpr static int k_derivandLayoutIndex = 0;
  constexpr static int k_variableLayoutIndex = 1;
  constexpr static int k_abscissaLayoutIndex = 2;

  DerivativeLayoutNode()
      : LayoutNode(), m_variableSlot(VariableSlot::Fraction) {}

  // LayoutNode
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode =
                    Preferences::PrintFloatMode::Decimal,
                int numberOfSignificantDigits = 0) const override;
  int indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction,
                                     int currentIndex,
                                     bool* shouldRedrawLayout) override;
  int indexAfterVerticalCursorMove(OMG::VerticalDirection direction,
                                   int currentIndex,
                                   PositionInLayout positionAtCurrentIndex,
                                   bool* shouldRedrawLayout) override;
  DeletionMethod deletionMethodForCursorLeftOfChild(
      int childIndex) const override;
  Layout XNTLayout(int childIndex = -1) const override;

  VariableSlot variableSlot() const { return m_variableSlot; }

 protected:
  constexpr static const char* k_d = "d";

  LayoutNode* derivandLayout() { return childAtIndex(k_derivandLayoutIndex); }
  LayoutNode* variableLayout() { return childAtIndex(k_variableLayoutIndex); }
  LayoutNode* abscissaLayout() { return childAtIndex(k_abscissaLayoutIndex); }

  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;

  virtual KDCoordinate orderHeightOffset(KDFont::Size font) = 0;
  virtual KDCoordinate orderWidth(KDFont::Size font) = 0;

  KDPoint positionOfVariableInFractionSlot(KDFont::Size font);
  KDPoint positionOfVariableInAssignmentSlot(KDFont::Size font);
  KDPoint positionOfDInNumerator(KDFont::Size font);
  KDPoint positionOfDInDenominator(KDFont::Size font);
  KDCoordinate abscissaBaseline(KDFont::Size font);
  KDCoordinate fractionBarWidth(KDFont::Size font);
  KDCoordinate parenthesesWidth(KDFont::Size font);

  void setVariableSlot(VariableSlot variableSlot, bool* shouldRecomputeLayout);

  constexpr static KDCoordinate k_dxHorizontalMargin = 2;
  constexpr static KDCoordinate k_barHorizontalMargin = 2;
  constexpr static KDCoordinate k_barWidth = 1;
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;

  /* There are two slots for the variable name: the Fraction and the Assignment
   * slots. This member is used to make the two copies of the variable name
   * interactive while storing the variable name only once. */
  VariableSlot m_variableSlot;
};

class FirstOrderDerivativeLayoutNode final : public DerivativeLayoutNode {
  // diff(f(x), x, a)
 public:
  // LayoutNode
  Type type() const override { return Type::FirstOrderDerivativeLayout; }
  int indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction,
                                     int currentIndex,
                                     bool* shouldRedrawLayout) override;
  int indexAfterVerticalCursorMove(OMG::VerticalDirection direction,
                                   int currentIndex,
                                   PositionInLayout positionAtCurrentIndex,
                                   bool* shouldRedrawLayout) override;

  // TreeNode
  size_t size() const override {
    return sizeof(FirstOrderDerivativeLayoutNode);
  }
  int numberOfChildren() const override { return k_numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "FirstOrderDerivativeLayout";
  }
#endif

 private:
  constexpr static int k_numberOfChildren = 3;

  KDCoordinate orderHeightOffset(KDFont::Size font) override { return 0; }
  KDCoordinate orderWidth(KDFont::Size font) override { return 0; }
};

class HigherOrderDerivativeLayoutNode final : public DerivativeLayoutNode {
  // diff(f(x), x, a, n)
 public:
  enum class OrderSlot : bool { Numerator, Denominator };

  HigherOrderDerivativeLayoutNode()
      : DerivativeLayoutNode(), m_orderSlot(OrderSlot::Denominator) {}
  // LayoutNode
  Type type() const override { return Type::HigherOrderDerivativeLayout; }
  int indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction,
                                     int currentIndex,
                                     bool* shouldRedrawLayout) override;
  int indexAfterVerticalCursorMove(OMG::VerticalDirection direction,
                                   int currentIndex,
                                   PositionInLayout positionAtCurrentIndex,
                                   bool* shouldRedrawLayout) override;

  // TreeNode
  size_t size() const override {
    return sizeof(HigherOrderDerivativeLayoutNode);
  }
  int numberOfChildren() const override { return k_numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "HigherOrderDerivativeLayout";
  }
#endif

 private:
  constexpr static int k_numberOfChildren = 4;
  constexpr static int k_orderLayoutIndex = k_numberOfChildren - 1;
  LayoutNode* orderLayout() { return childAtIndex(k_orderLayoutIndex); }

  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;
  KDPoint positionOfOrderInNumerator(KDFont::Size font);
  KDPoint positionOfOrderInDenominator(KDFont::Size font);

  KDCoordinate orderHeightOffset(KDFont::Size font) override {
    return orderLayout()->layoutSize(font).height() -
           VerticalOffsetLayoutNode::k_indiceHeight;
  }
  KDCoordinate orderWidth(KDFont::Size font) override {
    return orderLayout()->layoutSize(font).width();
  }

  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;

  void setOrderSlot(OrderSlot orderSlot, bool* shouldRecomputeLayout);

  /* There are two slots for the order of the derivative: the numerator and
   * the denominator slots. This member is used to make the two copies of the
   * order interactive while storing the order only once. */
  OrderSlot m_orderSlot;
};

class FirstOrderDerivativeLayout final
    : public LayoutThreeChildren<FirstOrderDerivativeLayout,
                                 FirstOrderDerivativeLayoutNode> {
 public:
  FirstOrderDerivativeLayout() = delete;
};

class HigherOrderDerivativeLayout final
    : public LayoutFourChildren<HigherOrderDerivativeLayout,
                                HigherOrderDerivativeLayoutNode> {
 public:
  HigherOrderDerivativeLayout() = delete;
};

}  // namespace Poincare

#endif
