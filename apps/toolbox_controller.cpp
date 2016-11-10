#include "toolbox_controller.h"
#include "toolbox_node.h"

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at nullptr. */

const ToolboxNode calculChildren[4] = {ToolboxNode("Nombre derivee", "diff(,)"), ToolboxNode("Integrale", "Int(,,)"), ToolboxNode("Somme", "sum(,,)"), ToolboxNode("Produit", "Product(,,)")};
const ToolboxNode complexChildren[5] = {ToolboxNode("Module", "abs()"), ToolboxNode("Argument", "arg()"), ToolboxNode("Partie reelle", "re()"), ToolboxNode("Partie imaginaire", "im()"), ToolboxNode("Conjugue", "conj()")};
const ToolboxNode probabilityChildren[4] = {ToolboxNode("Combinaison", "binomial()"), ToolboxNode("Arrangement", "permute(,)"), ToolboxNode("Nombre aleatoire", "random(,)"), ToolboxNode("Fonction gamma", "gamma()")};
const ToolboxNode arithmeticChildren[4] = {ToolboxNode("PGCD", "gcd()"), ToolboxNode("PPCM","lcm()"), ToolboxNode("Reste division euclidienne", "rem()"), ToolboxNode("Quotien division euclidienne", "quo()")};
const ToolboxNode matricesChildren[5] = {ToolboxNode("Inverse", "inverse()"), ToolboxNode("Determinant", "det()"), ToolboxNode("Transposee", "transpose()"), ToolboxNode("Trace", "trace()"), ToolboxNode("Taille", "dim()")};
const ToolboxNode listesChildren[5] = {ToolboxNode("Tri croissant", "sort<()"), ToolboxNode("Tri decroissant", "sort>()"), ToolboxNode("Maximum", "max()"), ToolboxNode("Minimum", "min()"), ToolboxNode("Taille", "dim()")};
const ToolboxNode approximationChildren[4] = {ToolboxNode("Partie entiere", "floor()"), ToolboxNode("Partie fractionnaire", "frac()"), ToolboxNode("Plafond", "ceil()"), ToolboxNode("Arrondi", "round(,)")};
const ToolboxNode trigonometryChildren[6] = {ToolboxNode("cosh", "cosh()"), ToolboxNode("sinh", "sinh()"), ToolboxNode("tanh", "tanh()"), ToolboxNode("acosh", "acosh()"), ToolboxNode("asinh", "asinh()"), ToolboxNode("atanh", "atanh()")};

const ToolboxNode menu[11] = {ToolboxNode("|x|", "abs()"), ToolboxNode("root(x)", "root(,)"), ToolboxNode("log(x)", "log(,)"),
  ToolboxNode("Calcul", nullptr, calculChildren, 4), ToolboxNode("Nombre complexe", nullptr, complexChildren, 5),
  ToolboxNode("Probabilite", nullptr, probabilityChildren, 4), ToolboxNode("Arithmetique", nullptr, arithmeticChildren, 4),
  ToolboxNode("Matrice",  nullptr, matricesChildren, 5), ToolboxNode("Liste", nullptr, listesChildren, 5),
  ToolboxNode("Approximation", nullptr, approximationChildren, 4), ToolboxNode("Trigonometrie", nullptr, trigonometryChildren, 6)};
const ToolboxNode toolboxModel = ToolboxNode("Toolbox", nullptr, menu, 11);

const char * ToolboxController::title() const {
  return "ToolboxController";
}

Node * ToolboxController::nodeModel() {
  return (Node *)&toolboxModel;
}

bool ToolboxController::selectLeaf(Node * selectedNode){
  m_listViewController.deselectTable();
  ToolboxNode * node = (ToolboxNode *)selectedNode;
  const char * editedText = node->text();
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
