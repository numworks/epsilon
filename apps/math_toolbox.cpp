#include "math_toolbox.h"
#include <assert.h>
#include <string.h>

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at nullptr. */

const ToolboxNode calculChildren[4] = {ToolboxNode("diff(,)", "Nombre derive"), ToolboxNode("int(,,)", "Integrale"), ToolboxNode("sum(,,)", "Somme"), ToolboxNode("product(,,)", "Produit")};
const ToolboxNode complexChildren[5] = {ToolboxNode("abs()", "Module"), ToolboxNode("arg()", "Argument"), ToolboxNode("re()", "Partie reelle"), ToolboxNode("im()", "Partie imaginaire"), ToolboxNode("conj()", "Conjugue")};
const ToolboxNode probabilityChildren[2] = {ToolboxNode("binomial(,)", "Combinaison"), ToolboxNode("permute(,)", "Arrangement")};
const ToolboxNode arithmeticChildren[4] = {ToolboxNode("gcd(,)", "PGCD"), ToolboxNode("lcm(,)", "PPCM"), ToolboxNode("rem(,)", "Reste division euclidienne"), ToolboxNode("quo(,)","Quotien division euclidienne")};
const ToolboxNode matricesChildren[5] = {ToolboxNode("inverse()", "Inverse"), ToolboxNode("det()", "Determinant"), ToolboxNode("transpose()", "Transposee"), ToolboxNode("trace()", "Trace"), ToolboxNode("dim()", "Taille")};
const ToolboxNode listesChildren[5] = {ToolboxNode("sort<()", "Tri croissant"), ToolboxNode("sort>()", "Tri decroissant"), ToolboxNode("max()", "Maximum"), ToolboxNode("min()", "Minimum"), ToolboxNode("dim()", "Taille")};
const ToolboxNode approximationChildren[4] = {ToolboxNode("floor()", "Partie entiere"), ToolboxNode("frac()", "Partie fractionnaire"), ToolboxNode("ceil()", "Plafond"), ToolboxNode("round(,)", "Arrondi")};
const ToolboxNode trigonometryChildren[6] = {ToolboxNode("cosh()", "cosh"), ToolboxNode("sinh()", "sinh"), ToolboxNode("tanh()", "tanh"), ToolboxNode("acosh()", "acosh"), ToolboxNode("asinh()", "asinh"), ToolboxNode("atanh()", "atanh")};
const ToolboxNode predictionChildren[3] = {ToolboxNode("prediction95(,)", "Intervalle fluctuation 95%"), ToolboxNode("prediction(,)", "Intervalle fluctuation simple"), ToolboxNode("confidence(,)", "Intervalle de confiance")};

const ToolboxNode menu[12] = {ToolboxNode("abs()", "Valeur absolue"), ToolboxNode("root(,)", "Racine k-ieme"), ToolboxNode("log(,)", "Logarithme base a"),
  ToolboxNode("Calcul", nullptr, calculChildren, 4), ToolboxNode("Nombre complexe", nullptr, complexChildren, 5),
  ToolboxNode("Probabilite", nullptr, probabilityChildren, 2), ToolboxNode("Arithmetique", nullptr, arithmeticChildren, 4),
  ToolboxNode("Matrice",  nullptr, matricesChildren, 5), ToolboxNode("Liste", nullptr, listesChildren, 5),
  ToolboxNode("Approximation", nullptr, approximationChildren, 4), ToolboxNode("Trigonometrie hyperbolique", nullptr, trigonometryChildren, 6),
  ToolboxNode("Intervalle fluctuation", nullptr, predictionChildren, 3)};
const ToolboxNode toolboxModel = ToolboxNode("Toolbox", nullptr, menu, 12);

/* State */

MathToolbox::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

int MathToolbox::Stack::State::selectedRow() {
  return m_selectedRow;
}

KDCoordinate MathToolbox::Stack::State::verticalScroll() {
  return m_verticalScroll;
}

bool MathToolbox::Stack::State::isNull(){
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */

void MathToolbox::Stack::push(int selectedRow, KDCoordinate verticalScroll) {
  int i = 0;
  while (!m_statesStack[i].isNull() && i < k_maxModelTreeDepth) {
    i++;
  }
  assert(m_statesStack[i].isNull());
  m_statesStack[i] = State(selectedRow, verticalScroll);
}

MathToolbox::Stack::State * MathToolbox::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

int MathToolbox::Stack::depth() {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

void MathToolbox::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return;
  }
  m_statesStack[stackDepth-1] = State();
}

void MathToolbox::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
}

/* List Controller */

MathToolbox::ListController::ListController(Responder * parentResponder, SelectableTableView * tableView) :
  ViewController(parentResponder),
  m_selectableTableView(tableView),
  m_firstSelectedRow(0)
{
}

View * MathToolbox::ListController::view() {
  return m_selectableTableView;
}

const char * MathToolbox::ListController::title() const {
  return toolboxModel.label();
}

void MathToolbox::ListController::didBecomeFirstResponder() {
  m_selectableTableView->reloadData();
  m_selectableTableView->selectCellAtLocation(0, m_firstSelectedRow);
  app()->setFirstResponder(m_selectableTableView);
}

void MathToolbox::ListController::setFirstSelectedRow(int firstSelectedRow) {
  m_firstSelectedRow = firstSelectedRow;
}

/* MathToolbox */

MathToolbox::MathToolbox() :
  Toolbox(nullptr, &m_listController),
  m_selectableTableView(SelectableTableView(&m_listController, this, 0, 0, 0, 0, nullptr, false)),
  m_listController(this, &m_selectableTableView),
  m_nodeModel(nullptr)
{
}

void MathToolbox::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_listController);
}

bool MathToolbox::handleEvent(Ion::Events::Event event) {
  return handleEventForRow(event, m_selectableTableView.selectedRow());
}

int MathToolbox::numberOfRows() {
  if (m_nodeModel == nullptr) {
    m_nodeModel = (ToolboxNode *)rootModel();
  }
  return m_nodeModel->numberOfChildren();
}

HighlightCell * MathToolbox::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  if (type == 0) {
    return &m_leafCells[index];
  }
  return &m_nodeCells[index];
}

int MathToolbox::reusableCellCount(int type) {
  return k_maxNumberOfDisplayedRows;
}

void MathToolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ToolboxNode * node = (ToolboxNode *)m_nodeModel->children(index);
  if (node->numberOfChildren() == 0) {
    PointerTableCellWithPointer * myCell = (PointerTableCellWithPointer *)cell;
    myCell->setText(node->label());
    myCell->setAccessoryText(node->text());
    myCell->setAccessoryTextColor(Palette::GreyDark);
    return;
  }
  PointerTableCell * myCell = (PointerTableCell *)cell;
  myCell->setText(node->label());
}

KDCoordinate MathToolbox::rowHeight(int j) {
  if (typeAtLocation(0, j) == 0) {
    return k_leafRowHeight;
  }
  return k_nodeRowHeight;
}

KDCoordinate MathToolbox::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int MathToolbox::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int MathToolbox::typeAtLocation(int i, int j) {
  Node * node = (Node *)m_nodeModel->children(j);
  if (node->numberOfChildren() == 0) {
    return 0;
  }
  return 1;
}

void MathToolbox::viewWillAppear() {
  Toolbox::viewWillAppear();
  m_nodeModel = (ToolboxNode *)rootModel();
  m_selectableTableView.reloadData();
  m_stack.resetStack();
  m_listController.setFirstSelectedRow(0);
}

void MathToolbox::viewWillDisappear() {
  m_selectableTableView.deselectTable();
}

int MathToolbox::stackDepth() {
  return m_stack.depth();
}

TextField * MathToolbox::sender() {
  return (TextField *)Toolbox::sender();
}

bool MathToolbox::handleEventForRow(Ion::Events::Event event, int selectedRow) {
  if (event == Ion::Events::Back) {
    return returnToPreviousMenu();
  }
  if (event == Ion::Events::OK) {
    ToolboxNode * selectedNode = (ToolboxNode *)m_nodeModel->children(selectedRow);
    if (selectedNode->numberOfChildren() == 0) {
      return selectLeaf(selectedNode);
    }
    return selectSubMenu(selectedNode);
  }
  return false;
}

const ToolboxNode * MathToolbox::rootModel() {
  return &toolboxModel;
}

bool MathToolbox::selectLeaf(ToolboxNode * selectedNode){
  m_selectableTableView.deselectTable();
  ToolboxNode * node = selectedNode;
  const char * editedText = node->label();
  if (!sender()->isEditing()) {
    sender()->setEditing(true);
  }
  sender()->insertTextAtLocation(editedText, sender()->cursorLocation());
  int cursorDelta = 0;
  int editedTextLength = strlen(editedText);
  for (int i = 0; i < editedTextLength; i++) {
    if (editedText[i] == '(') {
      cursorDelta =  i + 1;
      break;
    }
  }
  sender()->setCursorLocation(sender()->cursorLocation()+cursorDelta);
  app()->dismissModalViewController();
  return true;
}

bool MathToolbox::returnToPreviousMenu() {
  m_selectableTableView.deselectTable();
  int depth = m_stack.depth();
  if (depth == 0) {
    app()->dismissModalViewController();
    return true;
  }
  int index = 0;
  ToolboxNode * parentNode = (ToolboxNode *)rootModel();
  Stack::State * previousState = m_stack.stateAtIndex(index++);;
  while (depth-- > 1) {
    parentNode = (ToolboxNode *)parentNode->children(previousState->selectedRow());
    previousState = m_stack.stateAtIndex(index++);
  }
  m_selectableTableView.deselectTable();
  m_nodeModel = parentNode;
  m_listController.setFirstSelectedRow(previousState->selectedRow());
  KDPoint scroll = m_selectableTableView.contentOffset();
  m_selectableTableView.setContentOffset(KDPoint(scroll.x(), previousState->verticalScroll()));
  m_stack.pop();
  app()->setFirstResponder(&m_listController);
  return true;
}

bool MathToolbox::selectSubMenu(ToolboxNode * selectedNode) {
  m_stack.push(m_selectableTableView.selectedRow(),  m_selectableTableView.contentOffset().y());
  m_selectableTableView.deselectTable();
  m_nodeModel = selectedNode;
  m_listController.setFirstSelectedRow(0);
  app()->setFirstResponder(&m_listController);
  return true;
}
