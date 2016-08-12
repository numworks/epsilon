#include "list_controller.h"
#include <assert.h>

static const char * sMessages[] = {
  "AAA 0", "BBB 1", "CCC 2", "DDD 3", "EEE 4",
  "FFF 5", "GGG 6", "HHH 7", "III 8", "JJJ 9",
  "KKK10", "LLL11", "MMM12", "NNN13", "OOO14",
  "PPP15", "QQQ16", "RRR17", "SSS18", "TTT19"
};

ListController::ListController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_tableView(TableView(this)),
  m_activeCell(0),
  m_manualScrolling(0)
{
  m_messages = sMessages;
}

View * ListController::view() {
  return &m_tableView;
}

const char * ListController::title() const {
  return "Fonctions";
}

void ListController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfModels) {
    return;
  }

  m_activeCell = index;
  m_tableView.scrollToRow(index);
  FunctionCell * cell = (FunctionCell *)(m_tableView.cellAtIndex(index));
  cell->setParentResponder(this);
  App::runningApp()->focus(cell);
}

bool ListController::handleEvent(ion_event_t event) {
  switch (event) {
    case DOWN_ARROW:
      setActiveCell(m_activeCell+1);
      return true;
    case UP_ARROW:
      setActiveCell(m_activeCell-1);
      return true;
    case ENTER:
      m_manualScrolling += 10;
      m_tableView.setContentOffset({0, m_manualScrolling});
      return true;
    default:
      return false;
  }
}

int ListController::numberOfCells() {
  return k_totalNumberOfModels;
};

View * ListController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_maxNumberOfCells);
  return &m_cells[index];
}

int ListController::reusableCellCount() {
  return k_maxNumberOfCells;
}

void ListController::willDisplayCellForIndex(View * cell, int index) {
  FunctionCell * myCell = (FunctionCell *)cell;
  myCell->setMessage(m_messages[index]);
  myCell->setEven(index%2 == 0);
}

KDCoordinate ListController::cellHeight() {
  return 40;
}
