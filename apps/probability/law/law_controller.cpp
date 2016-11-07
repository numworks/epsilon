#include "law_controller.h"
#include <assert.h>
#include "../app.h"

static const char * sMessages[] = {
  "Loi Normale",
  "Exponentielle",
  "Student",
  "Khi 2",
  "Binomiale",
  "Poisson",
  "Geometrique"
};

Probability::LawController::LawController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin))
{
  m_messages = sMessages;
}

View * Probability::LawController::view() {
  return &m_selectableTableView;
}

const char * Probability::LawController::title() const {
  return "Type de Loi";
}

void Probability::LawController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool Probability::LawController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
      ((Probability::App *)app())->setLaw(App::Law::Normal);
      return true;
    default:
      return false;
  }
}

int Probability::LawController::numberOfRows() {
  return k_totalNumberOfModels;
};

TableViewCell * Probability::LawController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_maxNumberOfCells);
  return &m_cells[index];
}

int Probability::LawController::reusableCellCount() {
  return k_maxNumberOfCells;
}

void Probability::LawController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(m_messages[index]);
}

KDCoordinate Probability::LawController::cellHeight() {
  return 35;
}
