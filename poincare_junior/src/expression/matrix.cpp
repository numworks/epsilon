#include "matrix.h"

#include "k_tree.h"
#include "simplification.h"

namespace PoincareJ {

Tree* Matrix::Addition(const Tree* u, const Tree* v) {
  // should be an assert after dimensional analysis
  if (!(NumberOfRows(u) == NumberOfRows(v) &&
        NumberOfColumns(u) == NumberOfColumns(v))) {
    return KUndef->clone();
  }
  const Tree* childU = u->nextNode();
  const Tree* childV = v->nextNode();
  int n = u->numberOfChildren();
  Tree* result = u->cloneNode();
  for (int i = 0; i < n; i++) {
    Tree* child = KAdd.node<2>->cloneNode();
    childU->clone();
    childV->clone();
    Simplification::SimplifyAddition(child);
    childU = childU->nextTree();
    childV = childV->nextTree();
  }
  return result;
}

}  // namespace PoincareJ
