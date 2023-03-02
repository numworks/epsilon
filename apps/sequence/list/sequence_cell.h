#ifndef SEQUENCE_SEQUENCE_CELL_H
#define SEQUENCE_SEQUENCE_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>

#include "vertical_sequence_title_cell.h"

namespace Sequence {

class AbstractSequenceCell : public Escher::EvenOddCell {
 public:
  void setParameterSelected(bool selected);
  virtual Escher::EvenOddExpressionCell* expressionCell() = 0;
  VerticalSequenceTitleCell* titleCell() { return &m_sequenceTitleCell; }

 private:
  int numberOfSubviews() const override { return 2; }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  VerticalSequenceTitleCell m_sequenceTitleCell;
  bool m_parameterSelected;
};

class SequenceCell : public AbstractSequenceCell {
 public:
  Escher::EvenOddExpressionCell* expressionCell() override {
    return &m_expressionCell;
  }
  Escher::EvenOddExpressionCell m_expressionCell;
  // Escher::ExpressionView m_expressionView;
};

}  // namespace Sequence

#endif
