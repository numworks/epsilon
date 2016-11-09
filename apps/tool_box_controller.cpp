#include "tool_box_controller.h"
#include <assert.h>
#include <string.h>

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at nullptr. */

const Node calculChildren[4] = {Node("Nombre derivee", "diff(,)"), Node("Integrale", "Int(,,)"), Node("Somme", "sum(,,)"), Node("Produit", "Product(,,)")};
const Node complexChildren[5] = {Node("Module", "abs()"), Node("Argument", "arg()"), Node("Partie reelle", "re()"), Node("Partie imaginaire", "im()"), Node("Conjugue", "conj()")};
const Node probabilityChildren[4] = {Node("Combinaison", "binomial()"), Node("Arrangement", "permute(,)"), Node("Nombre aleatoire", "random(,)"), Node("Fonction gamma", "gamma()")};
const Node arithmeticChildren[4] = {Node("PGCD", "gcd()"), Node("PPCM","lcm()"), Node("Reste division euclidienne", "rem()"), Node("Quotien division euclidienne", "quo()")};
const Node matricesChildren[5] = {Node("Inverse", "inverse()"), Node("Determinant", "det()"), Node("Transposee", "transpose()"), Node("Trace", "trace()"), Node("Taille", "dim()")};
const Node listesChildren[5] = {Node("Tri croissant", "sort<()"), Node("Tri decroissant", "sort>()"), Node("Maximum", "max()"), Node("Minimum", "min()"), Node("Taille", "dim()")};
const Node approximationChildren[4] = {Node("Partie entiere", "floor()"), Node("Partie fractionnaire", "frac()"), Node("Plafond", "ceil()"), Node("Arrondi", "round(,)")};
const Node trigonometryChildren[6] = {Node("cosh", "cosh()"), Node("sinh", "sinh()"), Node("tanh", "tanh()"), Node("acosh", "acosh()"), Node("asinh", "asinh()"), Node("atanh", "atanh()")};

const Node menu[11] = {Node("|x|", "abs()"), Node("root(x)", "root(,)"), Node("log(x)", "log(,)"),
  Node("Calcul", nullptr, calculChildren, 4), Node("Nombre complexe", nullptr, complexChildren, 5),
  Node("Probabilite", nullptr, probabilityChildren, 4), Node("Arithmetique", nullptr, arithmeticChildren, 4),
  Node("Matrice",  nullptr,matricesChildren, 5), Node("Liste", nullptr, listesChildren, 5),
  Node("Approximation", nullptr, approximationChildren, 4), Node("Trigonometrie", nullptr, trigonometryChildren, 6)};
const Node toolBoxModel = Node("ToolBox", nullptr, menu, 11);

/* State */

ToolBoxController::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

int ToolBoxController::Stack::State::selectedRow() {
  return m_selectedRow;
}

KDCoordinate ToolBoxController::Stack::State::verticalScroll() {
  return m_verticalScroll;
}

bool ToolBoxController::Stack::State::isNull(){
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */

void ToolBoxController::Stack::push(int selectedRow, KDCoordinate verticalScroll) {
  int i = 0;
  while (!m_statesStack[i].isNull() && i < k_maxModelTreeDepth) {
    i++;
  }
  assert(m_statesStack[i].isNull());
  m_statesStack[i] = State(selectedRow, verticalScroll);
}

ToolBoxController::Stack::State * ToolBoxController::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

int ToolBoxController::Stack::depth() {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

void ToolBoxController::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return;
  }
  m_statesStack[stackDepth-1] = State();
}

void ToolBoxController::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
}

/* ToolBoxController */

ToolBoxController::ToolBoxController() :
  StackViewController(nullptr, &m_listViewController, true),
  m_listViewController(NodeListViewController(this))
{
}

const char * ToolBoxController::title() const {
  return "ToolBoxController";
}

bool ToolBoxController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ESC:
      return returnToPreviousMenu();
    case Ion::Events::Event::ENTER:
    {
      int selectedRow = m_listViewController.selectedRow();
      Node * selectedNode = (Node *)m_listViewController.nodeModel()->children(selectedRow);
      if (selectedNode->numberOfChildren() == 0) {
        return editMathFunction(selectedNode);
      }
      return selectSubMenu(selectedNode);
    }
    default:
      return false;
  }
}

bool ToolBoxController::returnToPreviousMenu() {
  m_listViewController.deselectTable();
  int depth = m_stack.depth();
  if (depth == 0) {
    app()->dismissModalViewController();
    return true;
  }
  int index = 0;
  Node * parentNode = (Node *)&toolBoxModel;
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

bool ToolBoxController::selectSubMenu(Node * selectedNode) {
  m_stack.push(m_listViewController.selectedRow(), m_listViewController.verticalScroll());
  m_listViewController.deselectTable();
  m_listViewController.setNodeModel(selectedNode);
  m_listViewController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listViewController);
  return true;
}

bool ToolBoxController::editMathFunction(Node * selectedNode){
  const char * editedText = selectedNode->text();
  m_textFieldCaller->appendText(editedText);
  int cursorPosition = 0;
  int editedTextLength = strlen(editedText);
  for (int i = 0; i < editedTextLength; i++) {
    if (editedText[i] == '(') {
      cursorPosition =  i + 1 - editedTextLength;
      break;
    }
  }
  m_textFieldCaller->moveCursor(cursorPosition);
  app()->dismissModalViewController();
  return true;
}

void ToolBoxController::didBecomeFirstResponder() {
  m_stack.resetStack();
  m_listViewController.setNodeModel((Node *)&toolBoxModel);
  m_listViewController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listViewController);
}

void ToolBoxController::setTextFieldCaller(TextField * textField) {
  m_textFieldCaller = textField;
}
