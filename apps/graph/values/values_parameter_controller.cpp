#include "values_parameter_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

ValuesParameterController::ValuesParameterController(Responder * parentResponder, Interval * interval) :
  ViewController(parentResponder),
  m_interval(interval),
  m_intervalStartCell(TextListViewCell((char*)"X Debut")),
  m_intervalEndCell(TextListViewCell((char*)"X Fin")),
  m_intervalStepCell(TextListViewCell((char*)"Pas")),
  m_listView(ListView(this,Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_activeCell(0)
{
}

const char * ValuesParameterController::title() const {
  return "Regler l'intervalle";
}

View * ValuesParameterController::view() {
  return &m_listView;
}

TextListViewCell * ValuesParameterController::ListViewCellAtIndex(int index) {
  switch(index) {
    case 0:
      return &m_intervalStartCell;
    case 1:
      return &m_intervalEndCell;
    case 2:
      return &m_intervalStepCell;
    default:
      assert(false);
      return nullptr;
  }
}

Graph::Interval * ValuesParameterController::interval() {
  return m_interval;
}

void ValuesParameterController::didBecomeFirstResponder() {
  setActiveCell(m_activeCell);
}

int ValuesParameterController::activeCell() {
  return m_activeCell;
}

void ValuesParameterController::willDisplayCellForIndex(View * cell, int index) {
  TextListViewCell * myCell = (TextListViewCell *) cell;
  char buffer[14];
  switch (index) {
    case 0:
      Float(m_interval->start()).convertFloatToText(buffer, 14, 7);
      myCell->setText(buffer);
      break;
    case 1:
      Float(m_interval->end()).convertFloatToText(buffer, 14, 7);
      myCell->setText(buffer);
      break;
    case 2:
      Float(m_interval->step()).convertFloatToText(buffer, 14, 7);
      myCell->setText(buffer);
      break;
    default:
      assert(false);
      return;
  }
}

void ValuesParameterController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfCell) {
    return;
  }
  TextListViewCell * previousCell = (TextListViewCell *)(m_listView.cellAtIndex(m_activeCell));
  previousCell->setHighlighted(false);

  m_activeCell = index;
  m_listView.scrollToRow(index);
  TextListViewCell * cell = (TextListViewCell *)(m_listView.cellAtIndex(index));
  cell->setHighlighted(true);
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
    case Ion::Events::Event::DOWN_ARROW:
      setActiveCell(m_activeCell+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCell-1);
      return true;
    case Ion::Events::Event::ENTER:
      editParameterInterval();
      return true;
    default:
      return false;
  }
}

void ValuesParameterController::editParameterInterval() {
  /* This code assumes that the active cell remains the one which is edited
   * until the invocation is performed. This could lead to concurrency issue in
   * other cases. */
  const char * initialTextContent = ListViewCellAtIndex(m_activeCell)->textContent();
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  inputController->edit(this, initialTextContent, this,
    [](void * context, void * sender){
    ValuesParameterController * valuesParameterController = (ValuesParameterController *)context;
    int activeCell = valuesParameterController->activeCell();
    TextListViewCell * cell = valuesParameterController->ListViewCellAtIndex(activeCell);
    InputViewController * myInputViewController = (InputViewController *)sender;
    const char * textBody = myInputViewController->textBody();
    App * myApp = (App *)valuesParameterController->app();
    Context * globalContext = myApp->globalContext();
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

View * ValuesParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  View * cells[] = {&m_intervalStartCell, &m_intervalEndCell, &m_intervalStepCell};
  return cells[index];
}

int ValuesParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ValuesParameterController::cellHeight() {
  return 35;
}

}
