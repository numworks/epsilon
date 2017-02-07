#ifndef SEQUENCE_SEQUENCE_EXPRESSION_CELL_H
#define SEQUENCE_SEQUENCE_EXPRESSION_CELL_H

#include "../sequence.h"
#include "../../shared/function_expression_cell.h"

namespace Sequence {

class SequenceExpressionCell : public Shared::FunctionExpressionCell, public Responder {
public:
  SequenceExpressionCell(Responder * parentResponder = nullptr);
  void setFunction(Shared::Function * f) override;
  int selectedSubCell();
  void selectSubCell(int index);
  void reloadCell() override;
  void setHighlighted(bool highlight) override;
  void setEven(bool even) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  Sequence * m_sequence;
  int m_numberOfSubCells;
  int m_selectedSubCell;
  EvenOddExpressionCell m_firstInitialConditionView;
  EvenOddExpressionCell m_secondInitialConditionView;
};

}

#endif
