#include <assert.h>
#include <escher/metric.h>
#include <escher/toolbox.h>
#include <string.h>

namespace Escher {

Toolbox::Toolbox(Responder *parentResponder, I18n::Message title)
    : NestedMenuController(parentResponder, title),
      m_messageTreeModel(nullptr) {}

int Toolbox::numberOfRows() const {
  if (m_messageTreeModel == nullptr) {
    m_messageTreeModel = rootModel();
  }
  return m_messageTreeModel->numberOfChildren();
}

int Toolbox::reusableCellCount(int type) { return maxNumberOfDisplayedRows(); }

void Toolbox::willDisplayCellForIndex(HighlightCell *cell, int index) {
  assert(typeAtIndex(index) == k_nodeCellType);
  const ToolboxMessageTree *messageTree = messageTreeModelAtIndex(index);
  assert(messageTree->numberOfChildren() != 0);
  MessageTableCell *myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(messageTree->label());
  myCell->reloadCell();
}

KDCoordinate Toolbox::nonMemoizedRowHeight(int index) {
  assert(typeAtIndex(index) == k_nodeCellType);
  MessageTableCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

int Toolbox::typeAtIndex(int index) const {
  if (messageTreeModelAtIndex(index)->numberOfChildren() == 0) {
    return k_leafCellType;
  }
  return k_nodeCellType;
}

bool Toolbox::selectSubMenu(int selectedRow) {
  assert(typeAtIndex(selectedRow) == k_nodeCellType);
  m_messageTreeModel = messageTreeModelAtIndex(selectedRow);
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool Toolbox::returnToPreviousMenu() {
  // Start from root to find parent messageTreeModel
  int currentDepth = stack()->length();
  int stateDepth = 0;
  m_messageTreeModel = rootModel();
  while (stateDepth < currentDepth - 1) {
    const StackState *previousState = stack()->elementAtIndex(stateDepth++);
    m_messageTreeModel = messageTreeModelAtIndex(previousState->selectedRow());
  }
  return NestedMenuController::returnToPreviousMenu();
}

bool Toolbox::returnToRootMenu() {
  m_messageTreeModel = rootModel();
  return NestedMenuController::returnToRootMenu();
}

const ToolboxMessageTree *Toolbox::messageTreeModelAtIndex(int index) const {
  assert(index >= 0 && index < m_messageTreeModel->numberOfChildren());
  const ToolboxMessageTree *messageTree =
      static_cast<const ToolboxMessageTree *>(
          m_messageTreeModel->childAtIndex(index));
  if (messageTree->isFork()) {
    messageTree = static_cast<const ToolboxMessageTree *>(
        messageTree->childAtIndex(indexAfterFork(messageTree)));
  }
  return messageTree;
}

}  // namespace Escher
