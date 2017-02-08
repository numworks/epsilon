#ifndef SEQUENCE_SEQUENCE_TITLE_CELL_H
#define SEQUENCE_SEQUENCE_TITLE_CELL_H

#include "../../shared/function_title_cell.h"
#include "../../shared/list_parameter_controller.h"
#include "../sequence.h"

namespace Sequence {

class SequenceTitleCell : public Shared::FunctionTitleCell, public Responder {
public:
  SequenceTitleCell(Responder * parentResponder, Shared::ListParameterController * listParameterController);
  void setSequence(Sequence * sequence);
  void setColor(KDColor color) override;
  int selectedSubCell();
  void selectSubCell(int index);
  void setHighlighted(bool highlight) override;
  void setEven(bool even) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  StackViewController * stackController();
  int m_numberOfSubCells;
  int m_selectedSubCell;
  EvenOddBufferTextCell m_definitionView;
  EvenOddBufferTextCell m_firstInitialConditionView;
  EvenOddBufferTextCell m_secondInitialConditionView;
  Sequence * m_sequence;
  Shared::ListParameterController * m_listParameterController;
};

}

#endif
