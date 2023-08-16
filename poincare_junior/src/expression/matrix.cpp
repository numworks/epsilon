#include "matrix.h"

#include "k_tree.h"
#include "simplification.h"

namespace PoincareJ {

Tree* Matrix::Identity(const Tree* n) {
  if (!n->block()->isNumber() || Integer::Handler(n).numberOfDigits() > 1) {
    return KUndef->clone();
  }
  uint8_t nb = *Integer::Handler(n).digits();
  Tree* result = SharedEditionPool->push<BlockType::Matrix>(nb, nb);
  for (int i = 0; i < nb - 1; i++) {
    (1_e)->clone();
    // cloning n zeros is indeed a memset(0)
    for (int j = 0; j < nb; j++) {
      (0_e)->clone();
    }
  }
  (1_e)->clone();
  return result;
}

Tree* Matrix::Trace(const Tree* matrix) {
  int n = NumberOfRows(matrix);
  if (n != NumberOfColumns(matrix)) {
    return KUndef->clone();
  }
  Tree* result = SharedEditionPool->push<BlockType::Addition>(n);
  const Tree* child = matrix->nextNode();
  for (int i = 0; i < n - 1; i++) {
    child->clone();
    for (int j = 0; j < n + 1; j++) {
      child = child->nextTree();
    }
  }
  child->clone();
  Simplification::SimplifyAddition(result);
  return result;
}

Tree* Matrix::Transpose(const Tree* m) {
  uint8_t rows = NumberOfRows(m);
  uint8_t cols = NumberOfColumns(m);
  if (rows == 1 || cols == 1) {
    Tree* result = m->clone();
    SetNumberOfRows(result, cols);
    SetNumberOfColumns(result, rows);
    return result;
  }
  Tree* result = SharedEditionPool->push<BlockType::Matrix>(cols, rows);
  const Tree* rowsM[rows];
  const Tree* child = m->nextNode();
  for (int row = 0; row < rows; row++) {
    rowsM[row] = child;
    for (int col = 0; col < cols; col++) {
      child = child->nextTree();
    }
  }
  for (int col = 0; col < cols; col++) {
    for (int row = 0; row < rows; row++) {
      rowsM[row]->clone();
      rowsM[row] = rowsM[row]->nextTree();
    }
  }
  return result;
}

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

Tree* Matrix::ScalarMultiplication(const Tree* scalar, const Tree* m) {
  Tree* result = m->cloneNode();
  for (const Tree* child : m->children()) {
    Tree* mult = SharedEditionPool->push<BlockType::Multiplication>(2);
    scalar->clone();
    child->clone();
    Simplification::SimplifyMultiplication(mult);
  }
  return result;
}

Tree* Matrix::Multiplication(const Tree* u, const Tree* v) {
  if (!(NumberOfColumns(u) == NumberOfRows(v))) {
    return KUndef->clone();
  }
  uint8_t rows = NumberOfRows(u);
  uint8_t internal = NumberOfColumns(u);
  uint8_t cols = NumberOfColumns(v);
  Tree* result = SharedEditionPool->push<BlockType::Matrix>(rows, cols);
  /* The complexity of the naive multiplication is n^3 by itself but if we do
   * not take care, indexing the children with childAtIndex will add an n^2
   * factor. To avoid this, we keep track of each row of v. */
  const Tree* rowsV[internal];
  {
    // Initialize row pointers
    const Tree* childV = v->nextNode();
    for (int k = 0; k < internal; k++) {
      rowsV[k] = childV;
      for (int c = 0; c < cols; c++) {
        childV = childV->nextTree();
      }
    }
  }
  const Tree* childURow0 = u->nextNode();
  const Tree* childURowK;
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      Tree* add = SharedEditionPool->push<BlockType::Addition, int>(internal);
      childURowK = childURow0;
      for (int k = 0; k < internal; k++) {
        Tree* mult = SharedEditionPool->push<BlockType::Multiplication>(2);
        assert(childURowK == ChildAtIndex(u, row, k));
        childURowK->clone();
        childURowK = childURowK->nextTree();
        assert(rowsV[k] == ChildAtIndex(v, k, col));
        rowsV[k]->clone();
        rowsV[k] = rowsV[k]->nextTree();
        Simplification::SimplifyMultiplication(mult);
      }
      Simplification::SimplifyAddition(add);
    }
    childURow0 = childURowK;
    // Since each row has moved cols times we can shift them to restore them
    for (int k = internal - 1; k > 0; k--) {
      rowsV[k] = rowsV[k - 1];
    }
    rowsV[0] = v->nextNode();
  }
  return result;
}

}  // namespace PoincareJ
