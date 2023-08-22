#include "matrix.h"

#include <float.h>
#include <poincare_junior/src/n_ary.h>

#include "approximation.h"
#include "k_tree.h"
#include "number.h"
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
  if (NumberOfRows(u) != NumberOfRows(v) ||
      NumberOfColumns(u) != NumberOfColumns(v)) {
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
  if (NumberOfColumns(u) != NumberOfRows(v)) {
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
        assert(childURowK == Child(u, row, k));
        childURowK->clone();
        childURowK = childURowK->nextTree();
        assert(rowsV[k] == Child(v, k, col));
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

bool Matrix::RowCanonize(Tree* matrix, bool reduced, Tree** determinant) {
  // The matrix children have to be reduced to be able to spot 0
  assert(!Simplification::DeepSystematicReduce(matrix));

  EditionReference det;
  if (determinant) {
    det = SharedEditionPool->push<BlockType::Multiplication>(0);
  }

  int m = NumberOfRows(matrix);
  int n = NumberOfColumns(matrix);

  int h = 0;  // row pivot
  int k = 0;  // column pivot

  while (h < m && k < n) {
    /* In non-reduced form, the pivot selection method will affect the output.
     * Here we prioritize the biggest pivot (in value) to get an output that
     * does not depends on the order of the rows of the matrix.
     * We could also take lowest non null pivots, or just first non null as we
     * already do with reduced forms. Output would be different, but correct. */
    int iPivot_temp = h;
    int iPivot = h;
    float bestPivot = 0.0;
    while (iPivot_temp < m) {
      // Using float to find the biggest pivot is sufficient.
      Tree* pivotChild = Child(matrix, iPivot_temp, k);
      float pivot = abs(Approximation::To<float>(pivotChild));
      // Handle very low pivots
      if (pivot == 0.0f && !Number::IsZero(pivotChild)) {
        pivot = FLT_MIN;
      }

      if (pivot > bestPivot) {
        // Update best pivot
        bestPivot = pivot;
        iPivot = iPivot_temp;
        if (reduced) {
          /* In reduced form, taking the first non null pivot is enough, and
           * more efficient. */
          break;
        }
      }
      iPivot_temp++;
    }
    /* TODO: Handle isNull == TrinaryBoolean::Unknown : rowCanonize will
     * output a mathematically wrong result (and divide expressions by a null
     * expression) if expression is actually null. For examples,
     * 1-cos(x)^2-sin(x)^2 would be mishandled. */
    if (Number::IsZero(Child(matrix, iPivot, k))) {
      // No non-null coefficient in this column, skip
      k++;
      if (determinant) {
        // Update determinant: det *= 0
        NAry::AddChild(det, (0_e)->clone());
      }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          SwapTrees(Child(matrix, iPivot, col), Child(matrix, h, col));
        }
        if (determinant) {
          // Update determinant: det *= -1
          NAry::AddChild(det, (-1_e)->clone());
        }
      }
      // Set to 1 M[h][k] by linear combination
      Tree* divisor = Child(matrix, h, k);
      if (determinant) {
        // Update determinant: det *= divisor
        NAry::AddChild(det, divisor->clone());
      }
      Tree* opHJ = divisor;
      for (int j = k + 1; j < n; j++) {
        opHJ = opHJ->nextTree();
        Tree* newOpHJ = SharedEditionPool->push<BlockType::Multiplication>(2);
        opHJ->clone();
        Tree* pow = SharedEditionPool->push<BlockType::Power>();
        divisor->clone();
        (-1_e)->clone();
        Simplification::ShallowSystematicReduce(pow);
        Simplification::ShallowSystematicReduce(newOpHJ);
        opHJ->moveTreeOverTree(newOpHJ);
        // TODO : Dependency
      }
      divisor->cloneTreeOverTree(1_e);

      int l = reduced ? 0 : h + 1;
      /* Set to 0 all M[i][j] i != h, j > k by linear combination. If a
       * non-reduced form is computed (ref), only rows below the pivot are
       * reduced, i > h as well */
      for (int i = l; i < m; i++) {
        if (i == h) {
          continue;
        }
        Tree* factor = Child(matrix, i, k);
        Tree* opIJ = factor;
        EditionReference opHJ = Child(matrix, h, k);  // opHJ may be after opIJ
        for (int j = k + 1; j < n; j++) {
          opIJ = opIJ->nextTree();
          opHJ = opHJ->nextTree();
          Tree* newOpIJ = SharedEditionPool->push<BlockType::Addition>(2);
          opIJ->clone();
          Tree* mult = SharedEditionPool->push<BlockType::Multiplication>(3);
          (-1_e)->clone();
          opHJ->clone();
          factor->clone();
          Simplification::ShallowSystematicReduce(mult);
          Simplification::ShallowSystematicReduce(newOpIJ);
          opIJ->moveTreeOverTree(newOpIJ);
          // TODO : Dependency
        }
        factor->cloneTreeOverTree(0_e);
      }
      h++;
      k++;
    }
  }
  if (determinant) {
    Simplification::ShallowSystematicReduce(det);
    *determinant = det;
  }
  return true;
}

int Matrix::Rank(const Tree* m) {
  Tree* copy = m->clone();
  RowCanonize(copy);
  int rank = NumberOfRows(copy);
  int i = rank - 1;
  while (i >= 0) {
    int j = NumberOfColumns(copy) - 1;
    // TODO: Handle TrinaryBoolean::Unknown. See rowCanonize comment
    while (j >= i && Number::IsZero(Child(copy, i, j))) {
      j--;
    }
    if (j <= i - 1) {
      rank--;
    } else {
      break;
    }
    i--;
  }
  copy->removeTree();
  return rank;
}

Tree* Matrix::Inverse(const Tree* m) {
  int dim = NumberOfRows(m);
  /* Create the matrix (A|I) with A is the input matrix and I the dim
   * identity matrix */
  Tree* matrixAI = SharedEditionPool->push<BlockType::Matrix>(dim, dim * 2);
  const Tree* childIJ = m->nextNode();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      childIJ->clone();
      childIJ = childIJ->nextTree();
    }
    for (int j = 0; j < dim; j++) {
      (i == j ? 1_e : 0_e)->clone();
    }
  }
  // Compute the inverse
  RowCanonize(matrixAI);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    if (!Number::IsOne(Child(matrixAI, i, i))) {
      matrixAI->removeTree();
      return KUndef->clone();
    }
  }
  // Remove A from (A|I)
  Tree* child = matrixAI->nextNode();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      child->removeTree();
    }
    for (int j = 0; j < dim; j++) {
      child = child->nextTree();
    }
  }
  SetNumberOfColumns(matrixAI, dim);
  return matrixAI;
}

Tree* Matrix::Power(const Tree* m, int p) {
  if (p < 0) {
    Tree* result = Power(m, -p);
    result->moveTreeOverTree(Inverse(result));
    return result;
  }
  if (p == 0) {
    Tree* result = IntegerHandler(NumberOfRows(m)).pushOnEditionPool();
    result->moveTreeOverTree(Identity(result));
    return result;
  }
  if (p == 1) {
    return m->clone();
  }
  if (p == 2) {
    return Multiplication(m, m);
  }
  // Quick exponentiation
  Tree* result = Matrix::Power(m, p / 2);
  result->moveTreeOverTree(Multiplication(result, result));
  if (p % 2 == 1) {
    result->moveTreeOverTree(Multiplication(m, result));
  }
  return result;
}

}  // namespace PoincareJ
