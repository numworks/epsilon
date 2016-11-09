#include "tool_box_controller.h"
#include "tool_box_node.h"

/* TODO: find a shorter way to initiate tree models
 * We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at nullptr. */

const ToolBoxNode calculChildren[4] = {ToolBoxNode("Nombre derivee", "diff(,)"), ToolBoxNode("Integrale", "Int(,,)"), ToolBoxNode("Somme", "sum(,,)"), ToolBoxNode("Produit", "Product(,,)")};
const ToolBoxNode complexChildren[5] = {ToolBoxNode("Module", "abs()"), ToolBoxNode("Argument", "arg()"), ToolBoxNode("Partie reelle", "re()"), ToolBoxNode("Partie imaginaire", "im()"), ToolBoxNode("Conjugue", "conj()")};
const ToolBoxNode probabilityChildren[4] = {ToolBoxNode("Combinaison", "binomial()"), ToolBoxNode("Arrangement", "permute(,)"), ToolBoxNode("Nombre aleatoire", "random(,)"), ToolBoxNode("Fonction gamma", "gamma()")};
const ToolBoxNode arithmeticChildren[4] = {ToolBoxNode("PGCD", "gcd()"), ToolBoxNode("PPCM","lcm()"), ToolBoxNode("Reste division euclidienne", "rem()"), ToolBoxNode("Quotien division euclidienne", "quo()")};
const ToolBoxNode matricesChildren[5] = {ToolBoxNode("Inverse", "inverse()"), ToolBoxNode("Determinant", "det()"), ToolBoxNode("Transposee", "transpose()"), ToolBoxNode("Trace", "trace()"), ToolBoxNode("Taille", "dim()")};
const ToolBoxNode listesChildren[5] = {ToolBoxNode("Tri croissant", "sort<()"), ToolBoxNode("Tri decroissant", "sort>()"), ToolBoxNode("Maximum", "max()"), ToolBoxNode("Minimum", "min()"), ToolBoxNode("Taille", "dim()")};
const ToolBoxNode approximationChildren[4] = {ToolBoxNode("Partie entiere", "floor()"), ToolBoxNode("Partie fractionnaire", "frac()"), ToolBoxNode("Plafond", "ceil()"), ToolBoxNode("Arrondi", "round(,)")};
const ToolBoxNode trigonometryChildren[6] = {ToolBoxNode("cosh", "cosh()"), ToolBoxNode("sinh", "sinh()"), ToolBoxNode("tanh", "tanh()"), ToolBoxNode("acosh", "acosh()"), ToolBoxNode("asinh", "asinh()"), ToolBoxNode("atanh", "atanh()")};

const ToolBoxNode menu[11] = {ToolBoxNode("|x|", "abs()"), ToolBoxNode("root(x)", "root(,)"), ToolBoxNode("log(x)", "log(,)"),
  ToolBoxNode("Calcul", nullptr, calculChildren, 4), ToolBoxNode("Nombre complexe", nullptr, complexChildren, 5),
  ToolBoxNode("Probabilite", nullptr, probabilityChildren, 4), ToolBoxNode("Arithmetique", nullptr, arithmeticChildren, 4),
  ToolBoxNode("Matrice",  nullptr, matricesChildren, 5), ToolBoxNode("Liste", nullptr, listesChildren, 5),
  ToolBoxNode("Approximation", nullptr, approximationChildren, 4), ToolBoxNode("Trigonometrie", nullptr, trigonometryChildren, 6)};
const ToolBoxNode toolBoxModel = ToolBoxNode("TOOLBOX", nullptr, menu, 11);

const char * ToolBoxController::title() const {
  return "ToolBoxController";
}

Node * ToolBoxController::nodeModel() {
  return (Node *)&toolBoxModel;
}

bool ToolBoxController::selectLeaf(Node * selectedNode){
  ToolBoxNode * node = (ToolBoxNode *)selectedNode;
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
