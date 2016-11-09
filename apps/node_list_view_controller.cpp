#include "tool_box_controller.h"
#include <assert.h>
#include <string.h>

NodeListViewController::NodeListViewController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this)),
  m_nodeModel(nullptr),
  m_firstSelectedRow(0)
{
}

View * NodeListViewController::view() {
  return &m_selectableTableView;
}

const char * NodeListViewController::title() const {
  return m_nodeModel->label();
}

Node * NodeListViewController::nodeModel() {
  return m_nodeModel;
}

void NodeListViewController::setNodeModel(Node * nodeModel) {
  m_nodeModel = nodeModel;
}

void NodeListViewController::setFirstSelectedRow(int firstSelectedRow) {
  m_firstSelectedRow = firstSelectedRow;
}

int NodeListViewController::selectedRow() {
  return m_selectableTableView.selectedRow();
}

void NodeListViewController::setVerticalScroll(KDCoordinate verticalScroll) {
  KDPoint scroll = m_selectableTableView.contentOffset();
  m_selectableTableView.setContentOffset(KDPoint(scroll.x(), verticalScroll));
}

KDCoordinate NodeListViewController::verticalScroll() {
  return m_selectableTableView.contentOffset().y();
}

void NodeListViewController::deselectTable() {
  m_selectableTableView.deselectTable();
}

void NodeListViewController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, m_firstSelectedRow);
  app()->setFirstResponder(&m_selectableTableView);
}

int NodeListViewController::numberOfRows() {
  return m_nodeModel->numberOfChildren();
}

TableViewCell * NodeListViewController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  if (type == 0) {
    return &m_commandCells[index];
  }
  return &m_menuCells[index];
}

int NodeListViewController::reusableCellCount(int type) {
  assert(type < 2);
  return k_maxNumberOfDisplayedRows;
}

void NodeListViewController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(m_nodeModel->children(index)->label());
}

KDCoordinate NodeListViewController::rowHeight(int j) {
  if (typeAtLocation(0, j) == 0) {
    return k_leafRowHeight;
  }
  return k_nodeRowHeight;
}

KDCoordinate NodeListViewController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int NodeListViewController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int NodeListViewController::typeAtLocation(int i, int j) {
  Node * node = (Node *)m_nodeModel->children(j);
  if (node->numberOfChildren() == 0) {
    return 0;
  }
  return 1;
}
