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
    MessageTableCellWithMessage * myCell = (MessageTableCellWithMessage *)cell;
    myCell->setMessage(messageTree->label());
    myCell->setAccessoryMessage(messageTree->text());
    myCell->setAccessoryTextColor(Palette::GreyDark);
    return;
  }
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(messageTree->label());
  myCell->reloadCell();
}

int Toolbox::typeAtLocation(int i, int j) {
  MessageTree * messageTree = (MessageTree *)m_messageTreeModel->childAtIndex(j);
  if (messageTree->numberOfChildren() == 0) {
    return LeafCellType;
  }
  return NodeCellType;
}

bool Toolbox::selectSubMenu(int selectedRow) {
  m_selectableTableView.deselectTable();
  m_messageTreeModel = (ToolboxMessageTree *)m_messageTreeModel->childAtIndex(selectedRow);
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool Toolbox::returnToPreviousMenu() {
  m_selectableTableView.deselectTable();
  int currentDepth = m_stack.depth();
  int index = 0;
  // We want to get the current ToolboxMessageTree's parent ToolboxMessageTree,
  // but there is no ToolboxMessageTree::getParent() method. We thus have to
  // start from the root ToolboxMessageTree and sequentially get the selected
  // child until it has the wanted depth.
  ToolboxMessageTree * parentMessageTree = (ToolboxMessageTree *)rootModel();
  Stack::State * previousState = m_stack.stateAtIndex(index++);
  while (currentDepth-- > 1) {
    parentMessageTree = (ToolboxMessageTree *)parentMessageTree->childAtIndex(previousState->selectedRow());
    previousState = m_stack.stateAtIndex(index++);
  }
  m_messageTreeModel = parentMessageTree;
  return NestedMenuController::returnToPreviousMenu();
}
