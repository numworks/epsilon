#include "values_parameter_controller.h"
#include "../app.h"
#include "../../constant.h"
#include "../../apps_container.h"
#include <assert.h>

namespace Graph {

ValuesParameterController::ValuesParameterController(Responder * parentResponder, Interval * interval) :
  ViewController(parentResponder),
  m_interval(interval),
  m_intervalStartCell(TextMenuListCell((char*)"X Debut")),
  m_intervalEndCell(TextMenuListCell((char*)"X Fin")),
  m_intervalStepCell(TextMenuListCell((char*)"Pas")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin))
{
}

const char * ValuesParameterController::title() const {
  return "Regler l'intervalle";
}

View * ValuesParameterController::view() {
  return &m_selectableTableView;
}

Graph::Interval * ValuesParameterController::interval() {
  return m_interval;
}

void ValuesParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

int ValuesParameterController::activeCell() {
  return m_selectableTableView.selectedRow();
}

void ValuesParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  TextMenuListCell * myCell = (TextMenuListCell *) cell;
  char buffer[Constant::FloatBufferSizeInScientificMode];
  switch (index) {
    case 0:
      Float(m_interval->start()).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
      myCell->setAccessoryText(buffer);
      break;
    case 1:
      Float(m_interval->end()).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
      myCell->setAccessoryText(buffer);
      break;
    case 2:
      Float(m_interval->step()).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
      myCell->setAccessoryText(buffer);
      break;
    default:
      assert(false);
      return;
  }
}

void ValuesParameterController::setIntervalParameterAtIndex(int parameterIndex, float f) {
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

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
      editInterval(false);
      return true;
    default:
      if ((int)event >= 0x100) {
        return false;
      }
      editInterval(true, (char)event);
      return true;
  }
}

void ValuesParameterController::editInterval(bool overwrite, char initialDigit) {
  /* This code assumes that the active cell remains the one which is edited
   * until the invocation is performed. This could lead to concurrency issue in
   * other cases. */
  char initialTextContent[Constant::FloatBufferSizeInScientificMode];
  if (overwrite) {
    initialTextContent[0] = initialDigit;
    initialTextContent[1] = 0;
  } else {
    TextMenuListCell * textMenuListCell = (TextMenuListCell *)reusableCell(activeCell());
    strlcpy(initialTextContent, textMenuListCell->accessoryText(), sizeof(initialTextContent));
  }
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  inputController->edit(this, initialTextContent, this,
    [](void * context, void * sender){
    ValuesParameterController * valuesParameterController = (ValuesParameterController *)context;
    int activeCell = valuesParameterController->activeCell();
    TextMenuListCell * cell = (TextMenuListCell *)valuesParameterController->reusableCell(activeCell);
    InputViewController * myInputViewController = (InputViewController *)sender;
    const char * textBody = myInputViewController->textBody();
    AppsContainer * appsContainer = (AppsContainer *)valuesParameterController->app()->container();
    Context * globalContext = appsContainer->context();
    float floatBody = Expression::parse(textBody)->approximate(*globalContext);
    valuesParameterController->setIntervalParameterAtIndex(activeCell, floatBody);
    valuesParameterController->willDisplayCellForIndex(cell, activeCell);
    },
    [](void * context, void * sender){
    });
}

int ValuesParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * ValuesParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_intervalStartCell, &m_intervalEndCell, &m_intervalStepCell};
  return cells[index];
}

int ValuesParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ValuesParameterController::cellHeight() {
  return 35;
}

}
