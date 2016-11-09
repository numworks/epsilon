#include "tool_box_controller.h"

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

const char * ToolBoxController::title() const {
  return "ToolBoxController";
}

Node * ToolBoxController::nodeModel() {
  return (Node *)&toolBoxModel;
}

bool ToolBoxController::selectLeaf(Node * selectedNode){
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
