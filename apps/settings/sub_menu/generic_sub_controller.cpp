#include "generic_sub_controller.h"

#include <assert.h>
#include <escher/container.h>
#include <escher/message_table_cell.h>

#include <cmath>

using namespace Poincare;
using namespace Escher;

namespace Settings {

GenericSubController::GenericSubController(Responder *parentResponder)
    : SelectableListViewController(parentResponder),
      m_messageTreeModel(nullptr) {}

const char *GenericSubController::title() {
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
  resetMemoization();
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

KDCoordinate GenericSubController::nonMemoizedRowHeight(int index) {
  MessageTableCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

void GenericSubController::willDisplayCellForIndex(HighlightCell *cell,
                                                   int index) {
  MessageTableCell *myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(m_messageTreeModel->childAtIndex(index)->label());
}

void GenericSubController::setMessageTreeModel(
    const MessageTree *messageTreeModel) {
  m_messageTreeModel = (MessageTree *)messageTreeModel;
}

void GenericSubController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
}

StackViewController *GenericSubController::stackController() const {
  return (StackViewController *)parentResponder();
}

}  // namespace Settings
