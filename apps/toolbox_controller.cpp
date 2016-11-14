#include "toolbox_controller.h"
#include "toolbox_node.h"

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at nullptr. */

const ToolboxNode calculChildren[4] = {ToolboxNode("diff(,)", "Nombre derivee"), ToolboxNode("Int(,,)", "Integrale"), ToolboxNode("sum(,,)", "Somme"), ToolboxNode("Product(,,)", "Produit")};
const ToolboxNode complexChildren[5] = {ToolboxNode("abs()", "Module"), ToolboxNode("arg()", "Argument"), ToolboxNode("re()", "Partie reelle"), ToolboxNode("im()", "Partie imaginaire"), ToolboxNode("conj()", "Conjugue")};
const ToolboxNode probabilityChildren[4] = {ToolboxNode("binomial()", "Combinaison"), ToolboxNode("permute(,)", "Arrangement"), ToolboxNode("random(,)", "Nombre aleatoire"), ToolboxNode("gamma()", "Fonction gamma")};
const ToolboxNode arithmeticChildren[4] = {ToolboxNode("gcd()", "PGCD"), ToolboxNode("lcm()", "PPCM"), ToolboxNode("rem()", "Reste division euclidienne"), ToolboxNode("quo()","Quotien division euclidienne")};
const ToolboxNode matricesChildren[5] = {ToolboxNode("inverse()", "Inverse"), ToolboxNode("det()", "Determinant"), ToolboxNode("transpose()", "Transposee"), ToolboxNode("trace()", "Trace"), ToolboxNode("dim()", "Taille")};
const ToolboxNode listesChildren[5] = {ToolboxNode("sort<()", "Tri croissant"), ToolboxNode("sort>()", "Tri decroissant"), ToolboxNode("max()", "Maximum"), ToolboxNode("min()", "Minimum"), ToolboxNode("dim()", "Taille")};
const ToolboxNode approximationChildren[4] = {ToolboxNode("floor()", "Partie entiere"), ToolboxNode("frac()", "Partie fractionnaire"), ToolboxNode("ceil()", "Plafond"), ToolboxNode("round(,)", "Arrondi")};
const ToolboxNode trigonometryChildren[6] = {ToolboxNode("cosh()", "cosh"), ToolboxNode("sinh()", "sinh"), ToolboxNode("tanh()", "tanh"), ToolboxNode("acosh()", "acosh"), ToolboxNode("asinh()", "asinh"), ToolboxNode("atanh()", "atanh")};

const ToolboxNode menu[11] = {ToolboxNode("abs()", "Valeur absolue"), ToolboxNode("root(,)", "Nombre derivee"), ToolboxNode("log(,)", "Logarithme base a"),
  ToolboxNode("Calcul", nullptr, calculChildren, 4), ToolboxNode("Nombre complexe", nullptr, complexChildren, 5),
  ToolboxNode("Probabilite", nullptr, probabilityChildren, 4), ToolboxNode("Arithmetique", nullptr, arithmeticChildren, 4),
  ToolboxNode("Matrice",  nullptr, matricesChildren, 5), ToolboxNode("Liste", nullptr, listesChildren, 5),
  ToolboxNode("Approximation", nullptr, approximationChildren, 4), ToolboxNode("Trigonometrie", nullptr, trigonometryChildren, 6)};
const ToolboxNode toolboxModel = ToolboxNode("Toolbox", nullptr, menu, 11);

const char * ToolboxController::title() const {
  return "ToolboxController";
}

TableViewCell * ToolboxController::leafCellAtIndex(int index) {
  return &m_leafCells[index];
}

TableViewCell * ToolboxController::nodeCellAtIndex(int index) {
  return & m_nodeCells[index];
}

void ToolboxController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  ToolboxNode * node = (ToolboxNode *)m_listViewController.nodeModel()->children(index);
  if (node->numberOfChildren() == 0) {
    ToolboxLeafCell * myCell = (ToolboxLeafCell *)cell;
    myCell->setLabel(node->label());
    myCell->setSubtitle(node->text());
    return;
  }
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(node->label());
}

KDCoordinate ToolboxController::leafRowHeight(int index) {
  return k_leafRowHeight;
}

Node * ToolboxController::nodeModel() {
  return (Node *)&toolboxModel;
}

bool ToolboxController::selectLeaf(Node * selectedNode){
  m_listViewController.deselectTable();
  ToolboxNode * node = (ToolboxNode *)selectedNode;
  const char * editedText = node->label();
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
