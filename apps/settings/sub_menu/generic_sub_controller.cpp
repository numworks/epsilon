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

void GenericSubController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void GenericSubController::viewWillAppear() {
  ViewController::viewWillAppear();
  /* This can't be done in didEnterResponderChain because we don't want it to
   * be done everytime the pop-up disappears. For example, if we are editing a
   * field and a pop-up shows up with a warning, we don't want to reload the
   * entire table when dismissing the pop-up (that would erase the edition). */
  selectCellAtLocation(0, initialSelectedRow());
  /* A unique SubController is used for all sub pages of settings. We have to
   * reload its data when it is displayed as it could switch from displaying
   * "Angle unit" data to "Complex format" data for instance. */
  m_selectableTableView.reloadData();
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
  MessageTableCell<> * myCell = (MessageTableCell<> *)cell;
  myCell->setMessage(m_messageTreeModel->childAtIndex(index)->label());
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
