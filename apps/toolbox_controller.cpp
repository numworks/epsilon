#include "toolbox_controller.h"
#include "toolbox_node.h"
#include <assert.h>

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at nullptr. */

const ToolboxNode calculChildren[4] = {ToolboxNode("diff(,)", "Nombre derive"), ToolboxNode("int(,,)", "Integrale"), ToolboxNode("sum(,,)", "Somme"), ToolboxNode("product(,,)", "Produit")};
const ToolboxNode complexChildren[5] = {ToolboxNode("abs()", "Module"), ToolboxNode("arg()", "Argument"), ToolboxNode("re()", "Partie reelle"), ToolboxNode("im()", "Partie imaginaire"), ToolboxNode("conj()", "Conjugue")};
const ToolboxNode probabilityChildren[4] = {ToolboxNode("binomial()", "Combinaison"), ToolboxNode("permute(,)", "Arrangement"), ToolboxNode("random(,)", "Nombre aleatoire"), ToolboxNode("gamma()", "Fonction gamma")};
const ToolboxNode arithmeticChildren[4] = {ToolboxNode("gcd()", "PGCD"), ToolboxNode("lcm()", "PPCM"), ToolboxNode("rem()", "Reste division euclidienne"), ToolboxNode("quo()","Quotien division euclidienne")};
const ToolboxNode matricesChildren[5] = {ToolboxNode("inverse()", "Inverse"), ToolboxNode("det()", "Determinant"), ToolboxNode("transpose()", "Transposee"), ToolboxNode("trace()", "Trace"), ToolboxNode("dim()", "Taille")};
const ToolboxNode listesChildren[5] = {ToolboxNode("sort<()", "Tri croissant"), ToolboxNode("sort>()", "Tri decroissant"), ToolboxNode("max()", "Maximum"), ToolboxNode("min()", "Minimum"), ToolboxNode("dim()", "Taille")};
const ToolboxNode approximationChildren[4] = {ToolboxNode("floor()", "Partie entiere"), ToolboxNode("frac()", "Partie fractionnaire"), ToolboxNode("ceil()", "Plafond"), ToolboxNode("round(,)", "Arrondi")};
const ToolboxNode trigonometryChildren[6] = {ToolboxNode("cosh()", "cosh"), ToolboxNode("sinh()", "sinh"), ToolboxNode("tanh()", "tanh"), ToolboxNode("acosh()", "acosh"), ToolboxNode("asinh()", "asinh"), ToolboxNode("atanh()", "atanh")};

const ToolboxNode menu[11] = {ToolboxNode("abs()", "Valeur absolue"), ToolboxNode("root(,)", "Racine k-ieme"), ToolboxNode("log(,)", "Logarithme base a"),
  ToolboxNode("Calcul", nullptr, calculChildren, 4), ToolboxNode("Nombre complexe", nullptr, complexChildren, 5),
  ToolboxNode("Probabilite", nullptr, probabilityChildren, 4), ToolboxNode("Arithmetique", nullptr, arithmeticChildren, 4),
  ToolboxNode("Matrice",  nullptr, matricesChildren, 5), ToolboxNode("Liste", nullptr, listesChildren, 5),
  ToolboxNode("Approximation", nullptr, approximationChildren, 4), ToolboxNode("Trigonometrie", nullptr, trigonometryChildren, 6)};
const ToolboxNode toolboxModel = ToolboxNode("Toolbox", nullptr, menu, 11);

/* State */

ToolboxController::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

int ToolboxController::Stack::State::selectedRow() {
  return m_selectedRow;
}

KDCoordinate ToolboxController::Stack::State::verticalScroll() {
  return m_verticalScroll;
}

bool ToolboxController::Stack::State::isNull(){
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */

void ToolboxController::Stack::push(int selectedRow, KDCoordinate verticalScroll) {
  int i = 0;
  while (!m_statesStack[i].isNull() && i < k_maxModelTreeDepth) {
    i++;
  }
  assert(m_statesStack[i].isNull());
  m_statesStack[i] = State(selectedRow, verticalScroll);
}

ToolboxController::Stack::State * ToolboxController::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

int ToolboxController::Stack::depth() {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

void ToolboxController::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return;
  }
  m_statesStack[stackDepth-1] = State();
}

void ToolboxController::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
}

/* ToolboxController */

ToolboxController::ToolboxController() :
  StackViewController(nullptr, &m_listViewController, true, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_listViewController(NodeListViewController(this))
{
}

const char * ToolboxController::title() const {
  return "ToolboxController";
}

bool ToolboxController::handleEvent(Ion::Events::Event event) {
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

void ToolboxController::didBecomeFirstResponder() {
  m_listViewController.setNodeModel(rootModel());
  StackViewController::didBecomeFirstResponder();
  m_stack.resetStack();
  m_listViewController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listViewController);
}

void ToolboxController::viewWillDisappear() {
  m_listViewController.deselectTable();
}

void ToolboxController::setTextFieldCaller(TextField * textField) {
  m_textFieldCaller = textField;
}

Node * ToolboxController::rootModel() {
  return (Node *)&toolboxModel;
}

bool ToolboxController::selectLeaf(Node * selectedNode){
  m_listViewController.deselectTable();
  ToolboxNode * node = (ToolboxNode *)selectedNode;
  const char * editedText = node->label();
  if (!m_textFieldCaller->isEditing()) {
    m_textFieldCaller->setEditing(true);
  }
  m_textFieldCaller->insertTextAtLocation(editedText, m_textFieldCaller->cursorLocation());
  int cursorDelta = 0;
  int editedTextLength = strlen(editedText);
  for (int i = 0; i < editedTextLength; i++) {
    if (editedText[i] == '(') {
      cursorDelta =  i + 1;
      break;
    }
  }
  m_textFieldCaller->setCursorLocation(m_textFieldCaller->cursorLocation()+cursorDelta);
  app()->dismissModalViewController();
  return true;
}

bool ToolboxController::returnToPreviousMenu() {
  m_listViewController.deselectTable();
  int depth = m_stack.depth();
  if (depth == 0) {
    app()->dismissModalViewController();
    return true;
  }
  int index = 0;
  Node * parentNode = rootModel();
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

bool ToolboxController::selectSubMenu(Node * selectedNode) {
  m_stack.push(m_listViewController.selectedRow(), m_listViewController.verticalScroll());
  m_listViewController.deselectTable();
  m_listViewController.setNodeModel(selectedNode);
  m_listViewController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listViewController);
  return true;
}
