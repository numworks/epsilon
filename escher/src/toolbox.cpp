#include <escher/toolbox.h>
#include <escher/metric.h>
#include <assert.h>
#include <string.h>

/* State */

Toolbox::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

bool Toolbox::Stack::State::isNull(){
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */

void Toolbox::Stack::push(int selectedRow, KDCoordinate verticalScroll) {
  int i = 0;
  while (!m_statesStack[i].isNull() && i < k_maxModelTreeDepth) {
    i++;
  }
  assert(m_statesStack[i].isNull());
  m_statesStack[i] = State(selectedRow, verticalScroll);
}

Toolbox::Stack::State * Toolbox::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

int Toolbox::Stack::depth() {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

void Toolbox::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return;
  }
  m_statesStack[stackDepth-1] = State();
}

void Toolbox::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
}

/* List Controller */

Toolbox::ListController::ListController(Responder * parentResponder, SelectableTableView * tableView, const char * title) :
  ViewController(parentResponder),
  m_selectableTableView(tableView),
  m_firstSelectedRow(0),
  m_title(title)
{
}

const char * Toolbox::ListController::title() {
  return m_title;
}

View * Toolbox::ListController::view() {
  return m_selectableTableView;
}

void Toolbox::ListController::didBecomeFirstResponder() {
  m_selectableTableView->reloadData();
  m_selectableTableView->selectCellAtLocation(0, m_firstSelectedRow);
  app()->setFirstResponder(m_selectableTableView);
}

void Toolbox::ListController::setFirstSelectedRow(int firstSelectedRow) {
  m_firstSelectedRow = firstSelectedRow;
}

/* Toolbox */

Toolbox::Toolbox(Responder * parentResponder, const char * title) :
  StackViewController(parentResponder, &m_listController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_selectableTableView(&m_listController, this, 0, 1, 0, 0, 0, 0, this, nullptr, false),
  m_listController(this, &m_selectableTableView, title),
  m_messageTreeModel(nullptr),
  m_sender(nullptr)
{
}

void Toolbox::setSender(Responder * sender) {
  m_sender = sender;
}

bool Toolbox::handleEvent(Ion::Events::Event event) {
  return handleEventForRow(event, selectedRow());
}

void Toolbox::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_listController);
}

void Toolbox::viewWillAppear() {
  StackViewController::viewWillAppear();
  m_messageTreeModel = (ToolboxMessageTree *)rootModel();
  m_selectableTableView.reloadData();
  m_stack.resetStack();
  m_listController.setFirstSelectedRow(0);
}

void Toolbox::viewDidDisappear() {
  StackViewController::viewDidDisappear();
  m_selectableTableView.deselectTable();
}

KDCoordinate Toolbox::rowHeight(int j) {
  return Metric::ToolboxRowHeight;
}

int Toolbox::numberOfRows() {
  if (m_messageTreeModel == nullptr) {
    m_messageTreeModel = (ToolboxMessageTree *)rootModel();
  }
  return m_messageTreeModel->numberOfChildren();
}

HighlightCell * Toolbox::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  assert(index < maxNumberOfDisplayedRows());
  if (type == LeafCellType) {
    return leafCellAtIndex(index);
  }
  return nodeCellAtIndex(index);
}

int Toolbox::reusableCellCount(int type) {
  return maxNumberOfDisplayedRows();
}

void Toolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ToolboxMessageTree * messageTree = (ToolboxMessageTree *)m_messageTreeModel->children(index);
  if (messageTree->numberOfChildren() == 0) {
    MessageTableCellWithMessage * myCell = (MessageTableCellWithMessage *)cell;
    myCell->setMessage(messageTree->label());
    myCell->setAccessoryMessage(messageTree->text());
    myCell->setAccessoryTextColor(Palette::GreyDark);
    return;
  }
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(messageTree->label());
}

KDCoordinate Toolbox::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int Toolbox::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int Toolbox::typeAtLocation(int i, int j) {
  MessageTree * messageTree = (MessageTree *)m_messageTreeModel->children(j);
  if (messageTree->numberOfChildren() == 0) {
    return LeafCellType;
  }
  return NodeCellType;
}

int Toolbox::stackDepth() {
  return m_stack.depth();
}

bool Toolbox::handleEventForRow(Ion::Events::Event event, int selectedRow) {
  int depth = m_stack.depth();
  if ((event == Ion::Events::Back || event == Ion::Events::Left) && depth > 0) {
    return returnToPreviousMenu();
  }
  ToolboxMessageTree * selectedMessageTree = (ToolboxMessageTree *)m_messageTreeModel->children(selectedRow);
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && selectedMessageTree->numberOfChildren() > 0) {
    return selectSubMenu(selectedMessageTree);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedMessageTree->numberOfChildren() == 0) {
    return selectLeaf(selectedMessageTree);
  }
  return false;
}

bool Toolbox::selectSubMenu(ToolboxMessageTree * selectedMessageTree) {
  m_stack.push(selectedRow(), m_selectableTableView.contentOffset().y());
  m_selectableTableView.deselectTable();
  m_messageTreeModel = selectedMessageTree;
  m_listController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listController);
  return true;
}

bool Toolbox::returnToPreviousMenu() {
  int currentDepth = m_stack.depth();
  int index = 0;
  // We want to get the current ToolboxMessageTree's parent ToolboxMessageTree,
  // but there is no ToolboxMessageTree::getParent() method. We thus have to
  // start from the root ToolboxMessageTree and sequentially get the selected
  // child until it has the wanted depth.
  ToolboxMessageTree * parentMessageTree = (ToolboxMessageTree *)rootModel();
  Stack::State * previousState = m_stack.stateAtIndex(index++);;
  while (currentDepth-- > 1) {
    parentMessageTree = (ToolboxMessageTree *)parentMessageTree->children(previousState->selectedRow());
    previousState = m_stack.stateAtIndex(index++);
  }
  m_selectableTableView.deselectTable();
  m_messageTreeModel = parentMessageTree;
  m_listController.setFirstSelectedRow(previousState->selectedRow());
  KDPoint scroll = m_selectableTableView.contentOffset();
  m_selectableTableView.setContentOffset(KDPoint(scroll.x(), previousState->verticalScroll()));
  m_stack.pop();
  app()->setFirstResponder(&m_listController);
  return true;
}

Responder * Toolbox::sender() {
  return m_sender;
}
