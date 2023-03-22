#include "generic_sub_controller.h"

#include <assert.h>
#include <escher/container.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

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

void GenericSubController::viewWillAppear() {
  ViewController::viewWillAppear();
  /* This can't be done in didEnterResponderChain because we don't want it to
   * be done everytime the pop-up disappears. For example, if we are editing a
   * field and a pop-up shows up with a warning, we don't want to reload the
   * entire table when dismissing the pop-up (that would erase the edition). */
  selectCell(initialSelectedRow());
  /* A unique SubController is used for all sub pages of settings. We have to
   * reload its data when it is displayed as it could switch from displaying
   * "Angle unit" data to "Complex format" data for instance. */
  resetMemoization();
  m_selectableListView.reloadData();
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
  MenuCell<MessageTextView> tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

void GenericSubController::willDisplayCellForIndex(HighlightCell *cell,
                                                   int index) {
  static_cast<MessageTextView *>(
      static_cast<AbstractMenuCell *>(cell)->widget(CellWidget::Type::Label))
      ->setMessage(m_messageTreeModel->childAtIndex(index)->label());
}

void GenericSubController::setMessageTreeModel(
    const MessageTree *messageTreeModel) {
  m_messageTreeModel = (MessageTree *)messageTreeModel;
}

void GenericSubController::viewDidDisappear() {
  m_selectableListView.deselectTable();
}

StackViewController *GenericSubController::stackController() const {
  return (StackViewController *)parentResponder();
}

}  // namespace Settings
