#include "controller.h"
extern "C" {
#include <assert.h>
}

namespace Home {

Controller::Controller(Responder * parentResponder) :
  ViewController(parentResponder),
  m_tableView(TableView(this))
{
}

View * Controller::view() {
  return &m_tableView;
}

int Controller::numberOfRows() {
  return 10;
}

int Controller::numberOfColumns() {
  return 3;
}

KDCoordinate Controller::cellHeight() {
  return 50;
}

KDCoordinate Controller::cellWidth() {
  return 50;
}

View * Controller::reusableCell(int index) {
  return &m_cells[index];
}

int Controller::reusableCellCount() {
  return k_maxNumberOfCells;
}

}
