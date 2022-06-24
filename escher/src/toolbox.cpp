#include <escher/toolbox.h>
#include <escher/metric.h>
#include <assert.h>
#include <string.h>

Toolbox::Toolbox(Responder * parentResponder, I18n::Message title) :
  NestedMenuController(parentResponder, title),
  m_messageTreeModel(nullptr)
{}

void Toolbox::viewWillAppear() {
  m_messageTreeModel = (ToolboxMessageTree *)rootModel();
  NestedMenuController::viewWillAppear();
}

int Toolbox::numberOfRows() const {
  if (m_messageTreeModel == nullptr) {
    m_messageTreeModel = (ToolboxMessageTree *)rootModel();
  }
  return m_messageTreeModel->numberOfChildren();
}

int Toolbox::reusableCellCount(int type) {
  return maxNumberOfDisplayedRows();
}

void Toolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ToolboxMessageTree * messageTree = (ToolboxMessageTree *)m_messageTreeModel->childAtIndex(index);
  if (messageTree->numberOfChildren() == 0) {
    MessageTableCellWithMessage<SlideableMessageTextView> * myCell = (MessageTableCellWithMessage<SlideableMessageTextView> *)cell;
    myCell->setMessage(messageTree->label());
    myCell->setAccessoryMessage(messageTree->text());
    myCell->setAccessoryTextColor(Palette::SecondaryText);
    return;
  } else {
    MessageTableCell<> * myCell = (MessageTableCell<> *)cell;
    myCell->setMessage(messageTree->label());
    myCell->reloadCell();
  }
}

int Toolbox::typeAtLocation(int i, int j) {
  const MessageTree * messageTree = m_messageTreeModel->childAtIndex(j);
  if (messageTree->numberOfChildren() == 0) {
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
  ToolboxMessageTree * parentMessageTree = (ToolboxMessageTree *)rootModel();
  Stack::State * previousState = m_stack.stateAtIndex(index++);
  while (currentDepth-- > 1) {
    parentMessageTree = (ToolboxMessageTree *)parentMessageTree->childAtIndex(previousState->selectedRow());
    previousState = m_stack.stateAtIndex(index++);
    if (parentMessageTree->isFork()) {
      parentMessageTree = (ToolboxMessageTree *)parentMessageTree->childAtIndex(indexAfterFork());
    }
  }
  m_messageTreeModel = parentMessageTree;
  return NestedMenuController::returnToPreviousMenu();
}
