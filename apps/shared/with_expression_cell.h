#ifndef SHARED_WITH_EXPRESSION_CELL_H
#define SHARED_WITH_EXPRESSION_CELL_H

#include <escher/editable_expression_model_cell.h>
#include <escher/even_odd_expression_cell.h>

namespace Shared {

/* These classes are used to factorize Sequence::SequenceCell /
 * Graph::FunctionCell and Sequence::EditableSequenceCell /
 * Graph::EditableFunctionCell */

class WithNonEditableExpressionCell {
 public:
  Escher::EvenOddExpressionCell* expressionCell() { return &m_expressionCell; }

 private:
  Escher::EvenOddExpressionCell m_expressionCell;
};

class WithEditableExpressionCell {
 public:
  WithEditableExpressionCell() : m_expressionCell() {}
  Escher::EditableExpressionModelCell* expressionCell() {
    return &m_expressionCell;
  }

 private:
  Escher::EditableExpressionModelCell m_expressionCell;
};

}  // namespace Shared

#endif
