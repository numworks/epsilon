#ifndef SEQUENCE_SEQUENCE_CELL_H
#define SEQUENCE_SEQUENCE_CELL_H

#include <apps/shared/with_expression_cell.h>
#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>

namespace Sequence {

class AbstractSequenceCell : public Escher::EvenOddCell {
 public:
  AbstractSequenceCell()
      : EvenOddCell(),
        m_functionColor(KDColorBlack),
        m_expressionBackground(KDColorWhite),
        m_ellipsisBackground(KDColorWhite),
        m_parameterSelected(false) {}
  void setParameterSelected(bool selected);
  virtual Escher::HighlightCell* mainCell() = 0;
  const Escher::HighlightCell* mainCell() const {
    return const_cast<Escher::HighlightCell*>(
        const_cast<AbstractSequenceCell*>(this)->mainCell());
  }

  void setColor(KDColor color) { m_functionColor = color; }

 private:
  int numberOfSubviews() const override { return 1 + displayEllipsis(); }
  Escher::View* subviewAtIndex(int index) override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;

 protected:
  constexpr static KDCoordinate k_verticalColorIndicatorThickness =
      Escher::Metric::VerticalColorIndicatorThickness;
  constexpr static KDCoordinate k_expressionMargin = 5;
  constexpr static KDCoordinate k_ellipsisWidth =
      Escher::Metric::EllipsisCellWidth;
  void setEven(bool even) override;
  virtual bool displayEllipsis() const { return true; }
  Escher::EllipsisView m_ellipsisView;
  KDColor m_functionColor;
  KDColor m_expressionBackground;
  KDColor m_ellipsisBackground;
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
  void updateSubviewsBackgroundAfterChangingState() override;

 private:
  void setEven(bool even) override;
};

}  // namespace Sequence

#endif
