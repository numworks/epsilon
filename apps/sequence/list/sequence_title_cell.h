#ifndef SEQUENCE_SEQUENCE_TITLE_CELL_H
#define SEQUENCE_SEQUENCE_TITLE_CELL_H

#include "../../shared/function_title_cell.h"
#include "../../shared/list_parameter_controller.h"
#include "../sequence.h"
#include "sequence_cell.h"

namespace Sequence {

class SequenceTitleCell : public SequenceCell, public Responder {
public:
  SequenceTitleCell(Responder * parentResponder, Shared::ListParameterController * listParameterController);
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
  EvenOddBufferTextCell m_definitionView;
  EvenOddBufferTextCell m_firstInitialConditionView;
  EvenOddBufferTextCell m_secondInitialConditionView;
  Shared::ListParameterController * m_listParameterController;
};

}

#endif
