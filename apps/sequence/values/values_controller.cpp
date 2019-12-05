#include "values_controller.h"
#include <assert.h>
#include <cmath>
#include "../../shared/poincare_helpers.h"
#include "../app.h"

using namespace Poincare;

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header),
  m_selectableTableView(this),
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
    /* No need to change Nstart/Nend messages because they are the only messages
     * used and we set them in ValuesController::ValuesController(...) */
    stack->push(controller);
    return true;
  }, this), k_font)
{
  for (int i = 0; i < k_maxNumberOfDisplayableSequences; i++) {
    m_sequenceTitleCells[i].setOrientation(Shared::FunctionTitleCell::Orientation::HorizontalIndicator);
  }
  setupSelectableTableViewAndCells(inputEventHandlerDelegate);
  setDefaultStartEndMessages();
}

// TableViewDataSource

KDCoordinate ValuesController::columnWidth(int i) {
  return k_cellWidth;
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

// AlternateEmptyViewDelegate
I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

// ValuesController
void ValuesController::setDefaultStartEndMessages() {
  m_intervalParameterController.setStartEndMessages(I18n::Message::NStart, I18n::Message::NEnd);
}

I18n::Message ValuesController::valuesParameterMessageAtColumn(int columnIndex) const {
  return I18n::Message::NColumn;
}

// EditableCellViewController

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  if (floatBody < 0) {
      return false;
  }
  return Shared::ValuesController::setDataAtLocation(std::round(floatBody), columnIndex, rowIndex);
}

// Model getters

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->interval();
}

// Function evaluation memoization

void ValuesController::fillMemoizedBuffer(int column, int row, int index) {
  char * buffer = memoizedBufferAtIndex(index);
  double abscissa = intervalAtColumn(column)->element(row-1); // Subtract the title row from row to get the element index
  Shared::ExpiringPointer<Sequence> sequence = functionStore()->modelForRecord(recordAtColumn(column));
  Coordinate2D<double> xy = sequence->evaluateXYAtParameter(abscissa, textFieldDelegateApp()->localContext());
  Shared::PoincareHelpers::ConvertFloatToText<double>(xy.x2(), buffer, k_valuesCellBufferSize, Preferences::LargeNumberOfSignificantDigits);
}

// Parameters controllers getter

ViewController * ValuesController::functionParameterController() {
#if COPY_COLUMN
  m_sequenceParameterController.setRecord(recordAtColumn(selectedColumn()));
  return &m_sequenceParameterController;
#else
  return nullptr;
#endif
}

}
