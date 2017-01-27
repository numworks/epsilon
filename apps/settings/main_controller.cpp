#include "main_controller.h"
#include <assert.h>

namespace Settings {

MainController::MainController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_cells{ChevronMenuListCell((char*)"Angles"), ChevronMenuListCell((char*)"Resultats"), ChevronMenuListCell((char*)"Forme nombre"),
    ChevronMenuListCell((char*)"Forme complexe"), ChevronMenuListCell((char*)"Langue")},
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin))
{
}

const char * MainController::title() const {
  return "Parametres";
}

View * MainController::view() {
  return &m_selectableTableView;
}

void MainController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() < 0) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  return false;
}

int MainController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * MainController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_cells[index];
}

int MainController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate MainController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}
