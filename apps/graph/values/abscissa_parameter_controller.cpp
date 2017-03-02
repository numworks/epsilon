#include "abscissa_parameter_controller.h"
#include <assert.h>

namespace Graph {

AbscissaParameterController::AbscissaParameterController(Responder * parentResponder, IntervalParameterController * intervalParameterController) :
  ViewController(parentResponder),
  m_deleteColumn(PointerTableCell((char*)"Effacer la colonne")),
  m_copyColumn(PointerTableCellWithChevron((char*)"Copier la colonne dans une liste")),
  m_setInterval(PointerTableCellWithChevron((char*)"Regler l'intervalle")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_intervalParameterController(intervalParameterController)
{
}

const char * AbscissaParameterController::title() const {
  return "Colonne x";
}

View * AbscissaParameterController::view() {
  return &m_selectableTableView;
}

void AbscissaParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool AbscissaParameterController::handleEvent(Ion::Events::Event event) {
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

int AbscissaParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * AbscissaParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_deleteColumn, &m_copyColumn, &m_setInterval};
  return cells[index];
}

int AbscissaParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate AbscissaParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}
