#include "sub_controller.h"
#include <assert.h>

namespace Settings {

SubController::SubController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_nodeModel(nullptr)
{
}

const char * SubController::title() const {
  if (m_nodeModel) {
    return m_nodeModel->label();
  }
  return "";
}

View * SubController::view() {
  return &m_selectableTableView;
}

void SubController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool SubController::handleEvent(Ion::Events::Event event) {
  return false;
}

int SubController::numberOfRows() {
  if (m_nodeModel) {
    return m_nodeModel->numberOfChildren();
  }
  return 0;
}

TableViewCell * SubController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_cells[index];
}

int SubController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate SubController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void SubController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(m_nodeModel->children(index)->label());
}

void SubController::setNodeModel(const Node * nodeModel) {
  m_nodeModel = (Node *)nodeModel;
}

}
