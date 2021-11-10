#include <escher/toolbox.h>
#include <escher/metric.h>
#include <assert.h>
#include <string.h>

namespace Escher {

Toolbox::Toolbox(Responder * parentResponder, I18n::Message title) :
  NestedMenuController(parentResponder, title),
  m_messageTreeModel(nullptr)
{}

void Toolbox::viewWillAppear() {
  m_messageTreeModel = rootModel();
  NestedMenuController::viewWillAppear();
}

int Toolbox::numberOfRows() const {
  if (m_messageTreeModel == nullptr) {
    m_messageTreeModel = rootModel();
  }
  return m_messageTreeModel->numberOfChildren();
}

int Toolbox::reusableCellCount(int type) {
  return maxNumberOfDisplayedRows();
}

void Toolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(typeAtIndex(index) == k_nodeCellType);
  const ToolboxMessageTree * messageTree = messageTreeModelAtIndex(index);
  assert(messageTree->numberOfChildren() != 0);
  MessageTableCell * myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(messageTree->label());
  myCell->reloadCell();
}

KDCoordinate Toolbox::nonMemoizedRowHeight(int index) {
  assert(typeAtIndex(index) == k_nodeCellType);
  MessageTableCell tempCell;
  return heightForCellAtIndex(&tempCell, index);
}

int Toolbox::typeAtIndex(int index) {
  if (messageTreeModelAtIndex(index)->numberOfChildren() == 0) {
    return k_leafCellType;
  }
  return k_nodeCellType;
}

bool Toolbox::selectSubMenu(int selectedRow) {
  m_selectableTableView.deselectTable();
  assert(typeAtIndex(selectedRow) == k_nodeCellType);
  m_messageTreeModel = messageTreeModelAtIndex(selectedRow);
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool Toolbox::returnToPreviousMenu() {
  m_selectableTableView.deselectTable();
  // Start from root to find parent messageTreeModel
  int currentDepth = stack()->depth();
  int stateDepth = 0;
  m_messageTreeModel = rootModel();
  while (stateDepth < currentDepth - 1) {
    const Stack::State * previousState = stack()->stateAtIndex(stateDepth++);
    m_messageTreeModel = messageTreeModelAtIndex(previousState->selectedRow());
  }
  return NestedMenuController::returnToPreviousMenu();
}

const ToolboxMessageTree * Toolbox::messageTreeModelAtIndex(int index) const {
  assert(index >= 0);
  const ToolboxMessageTree * messageTree = static_cast<const ToolboxMessageTree *>(m_messageTreeModel->childAtIndex(index));
  if (messageTree->isFork()) {
    messageTree = static_cast<const ToolboxMessageTree *>(messageTree->childAtIndex(indexAfterFork()));
  }
  return messageTree;
}

}
