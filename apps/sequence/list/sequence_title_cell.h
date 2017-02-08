#ifndef SEQUENCE_SEQUENCE_TITLE_CELL_H
#define SEQUENCE_SEQUENCE_TITLE_CELL_H

#include "../../shared/function_title_cell.h"
#include "list_parameter_controller.h"
#include "../sequence.h"
#include "sequence_cell.h"

namespace Sequence {

class SequenceTitleCell : public SequenceCell {
public:
  SequenceTitleCell(Responder * parentResponder, ListParameterController * listParameterController);
  ~SequenceTitleCell();
  void setSequence(Sequence * sequence) override;
  void setColor(KDColor color);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  EvenOddCell * viewAtIndex(int index) override;
  StackViewController * stackController();
  Shared::FunctionTitleCell m_backgroungCell;
  EvenOddExpressionCell m_definitionView;
  EvenOddExpressionCell m_firstInitialConditionView;
  EvenOddExpressionCell m_secondInitialConditionView;
  Poincare::ExpressionLayout * m_expressionLayouts[3];
  ListParameterController * m_listParameterController;
};

}

#endif
