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
  m_tableView(TableView(this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_activeCell(0)
{
  m_messages = sMessages;
}

View * Probability::LawController::view() {
  return &m_tableView;
}

const char * Probability::LawController::title() const {
  return "Type de Loi";
}

void Probability::LawController::didBecomeFirstResponder() {
  setActiveCell(0);
}

void Probability::LawController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfModels) {
    return;
  }
  ListViewCell * previousCell = (ListViewCell *)(m_tableView.cellAtLocation(0, m_activeCell));
  previousCell->setHighlighted(false);

  m_activeCell = index;
  m_tableView.scrollToCell(0, index);
  ListViewCell * cell = (ListViewCell *)(m_tableView.cellAtLocation(0, index));
  cell->setHighlighted(true);
}

bool Probability::LawController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      setActiveCell(m_activeCell+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCell-1);
      return true;
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
  ListViewCell * myCell = (ListViewCell *)cell;
  myCell->textView()->setText(m_messages[index]);
  if (m_activeCell == index) {
    myCell->textView()->setBackgroundColor(Palette::FocusCellBackgroundColor);
  } else {
    myCell->textView()->setBackgroundColor(Palette::CellBackgroundColor);
  }
  myCell->textView()->setTextColor(KDColorBlack);
  myCell->textView()->setAlignment(0., 0.5);
}

KDCoordinate Probability::LawController::cellHeight() {
  return 35;
}
