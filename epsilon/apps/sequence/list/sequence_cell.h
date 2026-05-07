#pragma once

#include <apps/shared/with_expression_cell.h>
#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>
#include <escher/theme/components.h>

namespace Sequence {

// TODO: factorize with AbstractFunctionCell which is very similar
class AbstractSequenceCell : public Escher::EvenOddCell {
 public:
  AbstractSequenceCell()
      : EvenOddCell(),
        m_functionColor(Escher::ThemeComponent::TextDefault),
        m_parameterSelected(false) {}
  void setParameterSelected(bool selected);
  virtual Escher::HighlightCell* mainCell() = 0;
  const Escher::HighlightCell* mainCell() const {
    return const_cast<Escher::HighlightCell*>(
        const_cast<AbstractSequenceCell*>(this)->mainCell());
  }

  void setColor(Escher::ThemeComponent color) { m_functionColor = color; }

  /* Background color logic is simpler than in EvenOddCell (no invisible cells)
   * and the highlight logic is resolved separately in the drawing function,
   * depending on which subview is selected (expression or ellipsis) */
  Escher::ThemeComponent backgroundColor() const override {
    return baseColor();
  }

  void setHighlighted(bool highlight) override;

 private:
  int numberOfSubviews() const override { return 1 + displayEllipsis(); }
  Escher::View* subviewAtIndex(int index) override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;

 protected:
  constexpr static KDCoordinate k_verticalColorIndicatorThickness =
      Escher::Metric::VerticalColorIndicatorThickness;
  constexpr static KDCoordinate k_margin =
      Escher::Metric::CellHorizontalElementMargin;
  constexpr static KDCoordinate k_ellipsisWidth =
      Escher::Metric::EllipsisCellWidth;
  virtual bool displayEllipsis() const { return true; }
  /* Note: in the base version expressionIsHighlighted(),
   * ellipsisIsHighlighted() and isHighlighted() mean the same thing, but it is
   * not the case in the SequenceCell override. */
  virtual bool expressionIsHighlighted() const { return isHighlighted(); }
  virtual bool ellipsisIsHighlighted() const { return isHighlighted(); }
  Escher::EllipsisView m_ellipsisView;
  Escher::ThemeComponent m_functionColor;
  bool m_parameterSelected;
};

template <typename T>
class TemplatedSequenceCell : public AbstractSequenceCell, public T {
 public:
  TemplatedSequenceCell() : AbstractSequenceCell(), T() {}

 private:
  Escher::HighlightCell* mainCell() override { return T::expressionCell(); }
};

class SequenceCell
    : public TemplatedSequenceCell<Shared::WithNonEditableExpressionCell> {
 public:
  bool expressionIsHighlighted() const override {
    return AbstractSequenceCell::expressionIsHighlighted() &&
           !m_parameterSelected;
  }
  bool ellipsisIsHighlighted() const override {
    return AbstractSequenceCell::ellipsisIsHighlighted() && m_parameterSelected;
  }

 private:
  void setEven(bool even) override;
};

}  // namespace Sequence
