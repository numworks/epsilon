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
  const ToolboxMessageTree * messageTree = static_cast<const ToolboxMessageTree *>(m_messageTreeModel->childAtIndex(index));
  assert(messageTree->numberOfChildren() != 0);
  MessageTableCell * myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(messageTree->label());
  myCell->reloadCell();
}

KDCoordinate Toolbox::nonMemoizedRowHeight(int index) {
  assert((m_messageTreeModel->childAtIndex(index))->numberOfChildren() != 0);
  MessageTableCell tempCell;
  return heightForCellAtIndex(&tempCell, index);
}

int Toolbox::typeAtIndex(int index) {
  if (m_messageTreeModel->childAtIndex(index)->numberOfChildren() == 0) {
    return LeafCellType;
  }
  return NodeCellType;
}

bool Toolbox::selectSubMenu(int selectedRow) {
  m_selectableTableView.deselectTable();
  m_messageTreeModel = static_cast<const ToolboxMessageTree *>(m_messageTreeModel->childAtIndex(selectedRow));
  if (m_messageTreeModel->isFork()) {
    assert(m_messageTreeModel->numberOfChildren() < 0);
    m_messageTreeModel = static_cast<const ToolboxMessageTree *>(m_messageTreeModel->childAtIndex(indexAfterFork()));
  }
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool Toolbox::returnToPreviousMenu() {
  m_selectableTableView.deselectTable();
  int currentDepth = m_stack.depth();
  int index = 0;
  ToolboxMessageTree * parentMessageTree = const_cast<ToolboxMessageTree *>(rootModel());
  Stack::State * previousState = m_stack.stateAtIndex(index++);
  while (currentDepth-- > 1) {
    parentMessageTree = static_cast<ToolboxMessageTree *>(const_cast<MessageTree *>(parentMessageTree->childAtIndex(previousState->selectedRow())));
    previousState = m_stack.stateAtIndex(index++);
    if (parentMessageTree->isFork()) {
      parentMessageTree = static_cast<ToolboxMessageTree *>(const_cast<MessageTree *>(parentMessageTree->childAtIndex(indexAfterFork())));
    }
  }
  m_messageTreeModel = parentMessageTree;
  return NestedMenuController::returnToPreviousMenu();
}

}
