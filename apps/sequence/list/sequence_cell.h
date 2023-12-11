#ifndef SEQUENCE_SEQUENCE_CELL_H
#define SEQUENCE_SEQUENCE_CELL_H

#include <apps/shared/with_expression_cell.h>
#include <escher/even_odd_cell.h>

#include "vertical_sequence_title_cell.h"

namespace Sequence {

class AbstractSequenceCell : public Escher::EvenOddCell {
 public:
  AbstractSequenceCell()
      : EvenOddCell(),
        m_expressionBackground(KDColorWhite),
        m_parameterSelected(false) {}
  void setParameterSelected(bool selected);
  virtual Escher::HighlightCell* mainCell() = 0;
  const Escher::HighlightCell* mainCell() const {
    return const_cast<Escher::HighlightCell*>(
        const_cast<AbstractSequenceCell*>(this)->mainCell());
  }
  VerticalSequenceTitleCell* titleCell() { return &m_sequenceTitleCell; }

 private:
  int numberOfSubviews() const override { return 2; }
  Escher::View* subviewAtIndex(int index) override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;

 protected:
  static constexpr KDCoordinate k_titlesColmunWidth = 65;
  void setEven(bool even) override;
  VerticalSequenceTitleCell m_sequenceTitleCell;
  KDColor m_expressionBackground;
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
