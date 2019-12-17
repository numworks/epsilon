#include "generic_sub_controller.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;

namespace Settings {

GenericSubController::GenericSubController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this),
  m_messageTreeModel(nullptr)
{
  m_selectableTableView.setTopMargin(k_topBottomMargin);
  m_selectableTableView.setBottomMargin(k_topBottomMargin);
}

const char * GenericSubController::title() {
  if (m_messageTreeModel) {
    return I18n::translate(m_messageTreeModel->label());
  }
  return "";
}

View * GenericSubController::view() {
  return &m_selectableTableView;
}

void GenericSubController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, initialSelectedRow());
  m_selectableTableView.reloadData();
}

void GenericSubController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool GenericSubController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

int GenericSubController::numberOfRows() const {
  if (m_messageTreeModel) {
    return m_messageTreeModel->numberOfChildren();
  }
  return 0;
}

KDCoordinate GenericSubController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate GenericSubController::cumulatedHeightFromIndex(int j) {
  return rowHeight(0) * j;
}

int GenericSubController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = rowHeight(0);
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

int GenericSubController::typeAtLocation(int i, int j) {
  return 0;
}

void GenericSubController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(m_messageTreeModel->children(index)->label());
}

void GenericSubController::setMessageTreeModel(const MessageTree * messageTreeModel) {
  m_messageTreeModel = (MessageTree *)messageTreeModel;
}

void GenericSubController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
}

StackViewController * GenericSubController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
