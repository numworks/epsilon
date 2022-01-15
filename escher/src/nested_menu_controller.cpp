#include <escher/nested_menu_controller.h>
#include <escher/container.h>
#include <escher/metric.h>
#include <assert.h>
#include <string.h>

/* State */

NestedMenuController::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

bool NestedMenuController::Stack::State::isNull() const {
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */

void NestedMenuController::Stack::push(int selectedRow, KDCoordinate verticalScroll) {
  int i = 0;
  while (!m_statesStack[i].isNull() && i < k_maxModelTreeDepth) {
    i++;
  }
  assert(m_statesStack[i].isNull());
  m_statesStack[i] = State(selectedRow, verticalScroll);
}

NestedMenuController::Stack::State * NestedMenuController::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

int NestedMenuController::Stack::depth() const {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

NestedMenuController::Stack::State NestedMenuController::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return State();
  }
  NestedMenuController::Stack::State state = m_statesStack[stackDepth-1];
  m_statesStack[stackDepth-1] = State();
  return state;
}

void NestedMenuController::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
}

/* List Controller */

NestedMenuController::ListController::ListController(Responder * parentResponder, SelectableTableView * tableView, I18n::Message title) :
  ViewController(parentResponder),
  m_selectableTableView(tableView),
  m_firstSelectedRow(0),
  m_title(title)
{
}

const char * NestedMenuController::ListController::title() {
  return I18n::translate(m_title);
}

View * NestedMenuController::ListController::view() {
  return m_selectableTableView;
}

void NestedMenuController::ListController::didBecomeFirstResponder() {
  m_selectableTableView->reloadData();
  Container::activeApp()->setFirstResponder(m_selectableTableView);
  m_selectableTableView->selectCellAtLocation(0, m_firstSelectedRow);
}

void NestedMenuController::ListController::setFirstSelectedRow(int firstSelectedRow) {
  m_firstSelectedRow = firstSelectedRow;
}

/* NestedMenuController */

NestedMenuController::NestedMenuController(Responder * parentResponder, I18n::Message title) :
  StackViewController(parentResponder, &m_listController, Palette::ToolboxHeaderText, Palette::ToolboxHeaderBackground, Palette::ToolboxHeaderBorder),
  m_selectableTableView(&m_listController, this, this, this),
  m_listController(this, &m_selectableTableView, title),
  m_sender(nullptr)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void NestedMenuController::setTitle(I18n::Message title) {
  m_listController.setTitle(title);
}

bool NestedMenuController::handleEvent(Ion::Events::Event event) {
  return handleEventForRow(event, selectedRow());
}

void NestedMenuController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
}

void NestedMenuController::viewWillAppear() {
  StackViewController::viewWillAppear();
  m_selectableTableView.reloadData();
  m_stack.resetStack();
  m_listController.setFirstSelectedRow(0);
}

void NestedMenuController::viewDidDisappear() {
  StackViewController::viewDidDisappear();
  m_selectableTableView.deselectTable();
}

KDCoordinate NestedMenuController::rowHeight(int j) {
  return Metric::ToolboxRowHeight;
}

HighlightCell * NestedMenuController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == LeafCellType) {
    return leafCellAtIndex(index);
  }
  return nodeCellAtIndex(index);
}

int NestedMenuController::stackDepth() const {
  return m_stack.depth();
}

bool NestedMenuController::handleEventForRow(Ion::Events::Event event, int rowIndex) {
  int depth = m_stack.depth();
  if ((event == Ion::Events::Back || event == Ion::Events::Left) && depth > 0) {
    return returnToPreviousMenu();
  }
  if ((event == Ion::Events::ShiftLeft || event == Ion::Events::AlphaLeft) && depth > 0) {
    for (int i = depth; i > 0; i--) {
      returnToPreviousMenu();
    }
    return true;
  }
  if (selectedRow() < 0) {
    return false;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && typeAtLocation(0, selectedRow()) == NodeCellType) {
    return selectSubMenu(rowIndex);
  }
  if (canStayInMenu() && ((event == Ion::Events::ShiftOK || event == Ion::Events::ShiftEXE) && typeAtLocation(0, selectedRow()) == LeafCellType)) {
    return selectLeaf(rowIndex, false);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && typeAtLocation(0, selectedRow()) == LeafCellType) {
    return selectLeaf(rowIndex, true);
  }
  return false;
}

bool NestedMenuController::selectSubMenu(int selectedRow) {
  m_stack.push(selectedRow, m_selectableTableView.contentOffset().y());
  m_listController.setFirstSelectedRow(0);
  Container::activeApp()->setFirstResponder(&m_listController);
  return true;
}

bool NestedMenuController::returnToPreviousMenu() {
  assert(m_stack.depth() > 0);
  NestedMenuController::Stack::State state = m_stack.pop();
  m_listController.setFirstSelectedRow(state.selectedRow() + stackRowOffset());
  KDPoint scroll = m_selectableTableView.contentOffset();
  m_selectableTableView.setContentOffset(KDPoint(scroll.x(), state.verticalScroll()));
  Container::activeApp()->setFirstResponder(&m_listController);
  return true;
}
