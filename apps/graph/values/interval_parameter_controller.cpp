#include "interval_parameter_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

IntervalParameterController::IntervalParameterController(Responder * parentResponder, Interval * interval) :
  FloatParameterController(parentResponder),
  m_interval(interval),
  m_intervalStartCell(EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char*)"X Debut")),
  m_intervalEndCell(EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char*)"X Fin")),
  m_intervalStepCell(EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char*)"Pas"))
{
}

ExpressionTextFieldDelegate * IntervalParameterController::textFieldDelegate() {
  ExpressionTextFieldDelegate * myApp = (ExpressionTextFieldDelegate *)app();
  return myApp;
}

const char * IntervalParameterController::title() const {
  return "Regler l'intervalle";
}

Graph::Interval * IntervalParameterController::interval() {
  return m_interval;
}

float IntervalParameterController::parameterAtIndex(int index) {
  switch (index) {
    case 0:
      return m_interval->start();
    case 1:
      return m_interval->end();
    case 2:
      return m_interval->step();
    default:
      assert(false);
      return 0.0f;
  }
}

void IntervalParameterController::setParameterAtIndex(int parameterIndex, float f) {
  switch(parameterIndex) {
    case 0:
      m_interval->setStart(f);
      break;
    case 1:
      m_interval->setEnd(f);
      break;
    case 2:
      m_interval->setStep(f);
      break;
    default:
      assert(false);
  }
}

int IntervalParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * IntervalParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_intervalStartCell, &m_intervalEndCell, &m_intervalStepCell};
  return cells[index];
}

int IntervalParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

bool IntervalParameterController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  App * myApp = (App *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

}
