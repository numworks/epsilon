#include "values_controller.h"
#include <assert.h>
#include <cmath>
#include "../app.h"

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header),
  m_sequenceTitleCells{},
  m_floatCells{},
  m_abscissaTitleCell(),
  m_abscissaCells{},
#if COPY_COLUMN
  m_sequenceParameterController('n'),
#endif
  m_intervalParameterController(this, inputEventHandlerDelegate),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    IntervalParameterController * controller = valuesController->intervalParameterController();
    controller->setInterval(valuesController->intervalAtColumn(valuesController->selectedColumn()));
    stack->push(controller);
    return true;
  }, this), k_font)
{
  for (int i = 0; i < k_maxNumberOfSequences; i++) {
    m_sequenceTitleCells[i].setOrientation(Shared::FunctionTitleCell::Orientation::HorizontalIndicator);
  }
  setupAbscissaCellsAndTitleCells(inputEventHandlerDelegate);
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
  if (typeAtLocation(i,j) == k_abscissaTitleCellType) {
    EvenOddMessageTextCell * mytitleCell = (EvenOddMessageTextCell *)cell;
    mytitleCell->setMessage(I18n::Message::N);
    return;
  }
  if (typeAtLocation(i,j) == k_functionTitleCellType) {
    SequenceTitleCell * myCell = (SequenceTitleCell *)cell;
    Sequence * sequence = functionStore()->modelForRecord(recordAtColumn(i));
    myCell->setLayout(sequence->nameLayout());
    myCell->setColor(sequence->color());
  }
}

I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  if (floatBody < 0) {
      return false;
  }
  return Shared::ValuesController::setDataAtLocation(std::round(floatBody), columnIndex, rowIndex);
}

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->interval();
}

ViewController * ValuesController::functionParameterController() {
#if COPY_COLUMN
  m_sequenceParameterController.setRecord(recordAtColumn(selectedColumn()));
  return &m_sequenceParameterController;
#else
  return nullptr;
#endif
}

I18n::Message ValuesController::valuesParameterControllerPageTitle() const {
  return I18n::Message::NColumn;
}

}
