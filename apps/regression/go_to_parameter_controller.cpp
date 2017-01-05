#include "go_to_parameter_controller.h"
#include <assert.h>

namespace Regression {

GoToParameterController::GoToParameterController(Responder * parentResponder, Store * store) :
  FloatParameterController(parentResponder),
  m_abscisseCell(EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer)),
  m_store(store),
  m_xPrediction(true)
{
}

void GoToParameterController::setXPrediction(bool xPrediction) {
  m_xPrediction = xPrediction;
}

const char * GoToParameterController::title() const {
  if (m_xPrediction) {
    return "Prediction sachant x";
  }
  return "Prediction sachant y";
}

float GoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  if (m_xPrediction) {
    return m_store->xCursorPosition();
  }
  return m_store->yCursorPosition();
}

void GoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  if (m_xPrediction) {
    m_store->setCursorPositionAtAbscissa(f);
  } else {
    m_store->setCursorPositionAtOrdinate(f);
  }
}

int GoToParameterController::numberOfRows() {
  return 1;
};

TableViewCell * GoToParameterController::reusableCell(int index) {
  assert(index == 0);
  return &m_abscisseCell;
}

int GoToParameterController::reusableCellCount() {
  return 1;
}

void GoToParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  EditableTextMenuListCell * myCell = (EditableTextMenuListCell *) cell;
  if (m_xPrediction) {
    myCell->setText("x");
  } else {
    myCell->setText("y");
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}


}
