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

ToolBoxController::ToolBoxController() :
  StackViewController(nullptr, this, true),
  m_selectableTableView(SelectableTableView(this, this)),
  m_toolBoxModel((Node *)&toolBoxModel)
{
}

const char * ToolBoxController::title() const {
  return "ToolBoxController";
}

bool ToolBoxController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Event::ENTER) {
    return false;
  }
  int selectedRow = m_selectableTableView.selectedRow();
  Node * selectedNode = (Node *) m_toolBoxModel->children(selectedRow);
  if (selectedNode->numberOfChildren() == 0) {
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
    m_selectableTableView.deselectTable();
    app()->dismissModalViewController();
    return true;
  }
  m_selectableTableView.deselectTable();
  m_toolBoxModel = (Node *)m_toolBoxModel->children(selectedRow);
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, 0);
  return true;
}

void ToolBoxController::didBecomeFirstResponder() {
  m_toolBoxModel = (Node *)&toolBoxModel;
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

int ToolBoxController::numberOfRows() {
  return m_toolBoxModel->numberOfChildren();
}

TableViewCell * ToolBoxController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  if (type == 0) {
    return &m_commandCells[index];
  }
  return &m_menuCells[index];
}

int ToolBoxController::reusableCellCount(int type) {
  assert(type < 2);
  return k_maxNumberOfDisplayedRows;
}

void ToolBoxController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(m_toolBoxModel->children(index)->label());
}

KDCoordinate ToolBoxController::rowHeight(int j) {
  if (typeAtLocation(0, j) == 0) {
    return k_commandRowHeight;
  }
  return k_menuRowHeight;
}

KDCoordinate ToolBoxController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int ToolBoxController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int ToolBoxController::typeAtLocation(int i, int j) {
  Node * node = (Node *)m_toolBoxModel->children(j);
  if (node->numberOfChildren() == 0) {
    return 0;
  }
  return 1;
}

void ToolBoxController::setTextFieldCaller(TextField * textField) {
  m_textFieldCaller = textField;
}
