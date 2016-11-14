#include "node_navigation_controller.h"
#include <assert.h>
#include <string.h>

/* State */

NodeNavigationController::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

int NodeNavigationController::Stack::State::selectedRow() {
  return m_selectedRow;
}

KDCoordinate NodeNavigationController::Stack::State::verticalScroll() {
  return m_verticalScroll;
}

bool NodeNavigationController::Stack::State::isNull(){
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */

void NodeNavigationController::Stack::push(int selectedRow, KDCoordinate verticalScroll) {
  int i = 0;
  while (!m_statesStack[i].isNull() && i < k_maxModelTreeDepth) {
    i++;
  }
  assert(m_statesStack[i].isNull());
  m_statesStack[i] = State(selectedRow, verticalScroll);
}

NodeNavigationController::Stack::State * NodeNavigationController::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

int NodeNavigationController::Stack::depth() {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

void NodeNavigationController::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return;
  }
  m_statesStack[stackDepth-1] = State();
}

void NodeNavigationController::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
}

/* NodeNavigationController */

NodeNavigationController::NodeNavigationController() :
  StackViewController(nullptr, &m_listViewController, true, KDColorWhite, Palette::BoxTitleBackgroundColor, Palette::BoxTitleBackgroundColor),
  m_listViewController(NodeListViewController(this))
{
}

const char * NodeNavigationController::title() const {
  return "NodeNavigationController";
}

bool NodeNavigationController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    return returnToPreviousMenu();
  }
  if (event == Ion::Events::OK) {
    int selectedRow = m_listViewController.selectedRow();
    Node * selectedNode = (Node *)m_listViewController.nodeModel()->children(selectedRow);
    if (selectedNode->numberOfChildren() == 0) {
      return selectLeaf(selectedNode);
    }
    return selectSubMenu(selectedNode);
  }
  return false;
}

bool NodeNavigationController::returnToPreviousMenu() {
  m_listViewController.deselectTable();
  int depth = m_stack.depth();
  if (depth == 0) {
    app()->dismissModalViewController();
    return true;
  }
  int index = 0;
  Node * parentNode = nodeModel();
  Stack::State * previousState = m_stack.stateAtIndex(index++);;
  while (depth-- > 1) {
    parentNode = (Node *)parentNode->children(previousState->selectedRow());
    previousState = m_stack.stateAtIndex(index++);
  }
  m_listViewController.deselectTable();
  m_listViewController.setNodeModel(parentNode);
  m_listViewController.setFirstSelectedRow(previousState->selectedRow());
  m_listViewController.setVerticalScroll(previousState->verticalScroll());
  m_stack.pop();
  app()->setFirstResponder(&m_listViewController);
  return true;
}

bool NodeNavigationController::selectSubMenu(Node * selectedNode) {
  m_stack.push(m_listViewController.selectedRow(), m_listViewController.verticalScroll());
  m_listViewController.deselectTable();
  m_listViewController.setNodeModel(selectedNode);
  m_listViewController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listViewController);
  return true;
}

void NodeNavigationController::didBecomeFirstResponder() {
  m_stack.resetStack();
  m_listViewController.setNodeModel(nodeModel());
  m_listViewController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listViewController);
}

void NodeNavigationController::setTextFieldCaller(TextField * textField) {
  m_textFieldCaller = textField;
}

void NodeNavigationController::willDisplayCellForIndex(TableViewCell * cell, int index) {
}
