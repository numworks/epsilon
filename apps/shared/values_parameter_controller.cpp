#include "values_parameter_controller.h"
#include <assert.h>

namespace Shared {

ValuesParameterController::ValuesParameterController(Responder * parentResponder, IntervalParameterController * intervalParameterController, char symbol) :
  ViewController(parentResponder),
  m_pageTitle("Colonne 0"),
  m_deleteColumn(PointerTableCell((char*)"Effacer la colonne")),
  m_copyColumn(PointerTableCellWithChevron((char*)"Copier la colonne dans une liste")),
  m_setInterval(PointerTableCellWithChevron((char*)"Regler l'intervalle")),
  m_selectableTableView(SelectableTableView(this, this, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_intervalParameterController(intervalParameterController)
{
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle; currentChar++) {
    if (m_pageTitle[currentChar] == ' ') {
      m_pageTitle[currentChar+1] = symbol;
      return;
    }
  }
}

const char * ValuesParameterController::title() const {
  return m_pageTitle;
}

View * ValuesParameterController::view() {
  return &m_selectableTableView;
}

void ValuesParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
      {
        Interval * interval = m_intervalParameterController->interval();
        interval->setEnd(0.0f);
        interval->setStep(1.0f);
        interval->setStart(1.0f);
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->pop();
        return true;
      }
      case 1:
        return false;
      case 2:
      {
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->push(m_intervalParameterController);
        return true;
      }
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

int ValuesParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * ValuesParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_deleteColumn, &m_copyColumn, &m_setInterval};
  return cells[index];
}

int ValuesParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ValuesParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}
