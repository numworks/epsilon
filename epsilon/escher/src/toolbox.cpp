#include <assert.h>
#include <escher/metric.h>
#include <escher/toolbox.h>
#include <string.h>

namespace Escher {

Toolbox::Toolbox(Responder* parentResponder, I18n::Message title)
    : NestedMenuController(parentResponder, title),
      m_messageTreeModel(nullptr) {}

int Toolbox::numberOfRows() const {
  if (m_messageTreeModel == nullptr) {
    m_messageTreeModel = rootModel();
  }
  return m_messageTreeModel->numberOfChildren();
}

int Toolbox::reusableCellCount(int type) const {
  return maxNumberOfDisplayedRows();
}

void Toolbox::fillCellForRow(HighlightCell* cell, int row) {
  assert(typeAtRow(row) == k_nodeCellType);
  const ToolboxMessageTree* messageTree = messageTreeModelAtIndex(row);
  assert(messageTree->numberOfChildren() != 0);
  NestedMenuController::NodeCell* myCell =
      static_cast<NestedMenuController::NodeCell*>(cell);
  myCell->label()->setMessage(messageTree->label());
  myCell->reloadCell();
}

KDCoordinate Toolbox::nonMemoizedRowHeight(int row) {
  assert(typeAtRow(row) == k_nodeCellType);
  NestedMenuController::NodeCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

int Toolbox::typeAtRow(int row) const {
  if (messageTreeModelAtIndex(row)->numberOfChildren() == 0) {
    return k_leafCellType;
  }
  return k_nodeCellType;
}

bool Toolbox::selectSubMenu(int selectedRow) {
  assert(typeAtRow(selectedRow) == k_nodeCellType);
  m_messageTreeModel = messageTreeModelAtIndex(selectedRow);
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool Toolbox::returnToPreviousMenu() {
  // Start from root to find parent messageTreeModel
  int currentDepth = stack()->length();
  int stateDepth = 0;
  m_messageTreeModel = rootModel();
  while (stateDepth < currentDepth - 1) {
    const StackState* previousState = stack()->elementAtIndex(stateDepth++);
    m_messageTreeModel = messageTreeModelAtIndex(previousState->selectedRow());
  }
  return NestedMenuController::returnToPreviousMenu();
}

bool Toolbox::returnToRootMenu() {
  m_messageTreeModel = rootModel();
  return NestedMenuController::returnToRootMenu();
}

const ToolboxMessageTree* Toolbox::messageTreeModelAtIndex(int index) const {
  assert(index >= 0 && index < m_messageTreeModel->numberOfChildren());
  const ToolboxMessageTree* messageTree =
      static_cast<const ToolboxMessageTree*>(
          m_messageTreeModel->childAtIndex(index));
  if (messageTree->isFork()) {
    messageTree = static_cast<const ToolboxMessageTree*>(
        messageTree->childAtIndex(indexAfterFork(messageTree)));
  }
  return messageTree;
}

}  // namespace Escher
