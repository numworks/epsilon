#include "matrix.h"

#include <float.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

#include "approximation.h"
#include "k_tree.h"
#include "number.h"
#include "simplification.h"
#include "vector.h"

namespace PoincareJ {

Tree* Matrix::Zero(MatrixDimension d) {
  Tree* result = SharedEditionPool->push<BlockType::Matrix>(d.rows, d.cols);
  for (int i = 0; i < d.rows * d.cols; i++) {
    (0_e)->clone();
  }
  return result;
}

Tree* Matrix::Identity(const Tree* n) {
  assert(n->isNumber());
  if (Integer::Handler(n).numberOfDigits() > 1) {
    ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
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

Tree* Matrix::Trace(const Tree* matrix, bool approximate) {
  int n = NumberOfRows(matrix);
  assert(n == NumberOfColumns(matrix));
  Tree* result = SharedEditionPool->push<BlockType::Addition>(n);
  const Tree* child = matrix->nextNode();
  for (int i = 0; i < n - 1; i++) {
    child->clone();
    for (int j = 0; j < n + 1; j++) {
      child = child->nextTree();
    }
  }
  child->clone();
  if (approximate) {
    Approximation::SimplifyComplex(result);
  } else {
    Simplification::SimplifyAddition(result);
  }
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

Tree* Matrix::Addition(const Tree* u, const Tree* v, bool approximate) {
  // should be an assert after dimensional analysis
  assert(NumberOfRows(u) == NumberOfRows(v) &&
         NumberOfColumns(u) == NumberOfColumns(v));
  const Tree* childU = u->nextNode();
  const Tree* childV = v->nextNode();
  int n = u->numberOfChildren();
  Tree* result = u->cloneNode();
  for (int i = 0; i < n; i++) {
    Tree* child = KAdd.node<2>->cloneNode();
    childU->clone();
    childV->clone();
    if (approximate) {
      Approximation::SimplifyComplex(child);
    } else {
      Simplification::SimplifyAddition(child);
    }
    childU = childU->nextTree();
    childV = childV->nextTree();
  }
  return result;
}

Tree* Matrix::ScalarMultiplication(const Tree* scalar, const Tree* m,
                                   bool approximate) {
  Tree* result = m->cloneNode();
  for (const Tree* child : m->children()) {
    Tree* mult = SharedEditionPool->push<BlockType::Multiplication>(2);
    scalar->clone();
    child->clone();
    if (approximate) {
      Approximation::SimplifyComplex(mult);
    } else {
      Simplification::SimplifyMultiplication(mult);
    }
  }
  return result;
}

Tree* Matrix::Multiplication(const Tree* u, const Tree* v, bool approximate) {
  assert(NumberOfColumns(u) == NumberOfRows(v));
  uint8_t rows = NumberOfRows(u);
  uint8_t internal = NumberOfColumns(u);
  uint8_t cols = NumberOfColumns(v);
  Tree* result = SharedEditionPool->push<BlockType::Matrix>(rows, cols);
  /* The complexity of the naive multiplication is n^3 by itself but if we do
   * not take care, indexing the children with child will add an n^2
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
        if (approximate) {
          Approximation::SimplifyComplex(mult);
        } else {
          Simplification::SimplifyMultiplication(mult);
        }
      }
      if (approximate) {
        Approximation::SimplifyComplex(add);
      } else {
        Simplification::SimplifyAddition(add);
      }
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

bool Matrix::RowCanonize(Tree* matrix, bool reduced, Tree** determinant,
                         bool approximate) {
  // The matrix children have to be reduced to be able to spot 0
  assert(approximate || !Simplification::DeepSystematicReduce(matrix));

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
      float pivot = std::abs(Approximation::ToComplex<float>(pivotChild));
      // Handle very low pivots
      if (pivot == 0.0f && !pivotChild->isZero()) {
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
    Tree* candidate = Child(matrix, iPivot, k);
    if (candidate->isZero() ||
        (approximate &&
         std::abs(Approximation::ToComplex<float>(candidate)) == 0)) {
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
          Child(matrix, iPivot, col)->swapWithTree(Child(matrix, h, col));
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
        if (approximate) {
          Tree* newOpHJ = PatternMatching::Create(KMult(KA, KPow(KB, -1_e)),
                                                  {.KA = opHJ, .KB = divisor});
          Approximation::SimplifyComplex(newOpHJ);
          opHJ->moveTreeOverTree(newOpHJ);
        } else {
          opHJ->moveTreeOverTree(PatternMatching::CreateAndSimplify(
              KMult(KA, KPow(KB, -1_e)), {.KA = opHJ, .KB = divisor}));
        }
        // TODO_PCJ : Dependency
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
          if (approximate) {
            Tree* newOpIJ =
                PatternMatching::Create(KAdd(KA, KMult(-1_e, KB, KC)),
                                        {.KA = opIJ, .KB = opHJ, .KC = factor});
            Approximation::SimplifyComplex(newOpIJ);
            opIJ->moveTreeOverTree(newOpIJ);
          } else {
            opIJ->moveTreeOverTree(PatternMatching::CreateAndSimplify(
                KAdd(KA, KMult(-1_e, KB, KC)),
                {.KA = opIJ, .KB = opHJ, .KC = factor}));
          }
          // TODO_PCJ : Dependency
        }
        factor->cloneTreeOverTree(0_e);
      }
      h++;
      k++;
    }
  }
  if (determinant) {
    if (approximate) {
      Approximation::SimplifyComplex(det);
    } else {
      Simplification::ShallowSystematicReduce(det);
    }
    *determinant = det;
  }
  return true;
}

int Matrix::Rank(const Tree* m) {
  Tree* copy = m->clone();
  RowCanonize(copy);
  int rank = RankOfCanonized(copy);
  copy->removeTree();
  return rank;
}

int Matrix::CanonizeAndRank(Tree* m) {
  RowCanonize(m);
  return RankOfCanonized(m);
}

int Matrix::RankOfCanonized(const Tree* m) {
  int rank = NumberOfRows(m);
  int i = rank - 1;
  while (i >= 0) {
    int j = NumberOfColumns(m) - 1;
    // TODO: Handle TrinaryBoolean::Unknown. See rowCanonize comment
    while (j >= i && Child(m, i, j)->isZero()) {
      j--;
    }
    if (j <= i - 1) {
      rank--;
    } else {
      break;
    }
    i--;
  }
  return rank;
}

Tree* Matrix::Inverse(const Tree* m, bool approximate) {
  assert(NumberOfRows(m) == NumberOfColumns(m));
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
  RowCanonize(matrixAI, true, nullptr, approximate);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    if (!Child(matrixAI, i, i)->isOne()) {
      ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
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

Tree* Matrix::Power(const Tree* m, int p, bool approximate) {
  assert(NumberOfRows(m) == NumberOfColumns(m));
  if (p < 0) {
    Tree* result = Power(m, -p);
    // TODO is it worth to compute inverse first ?
    result->moveTreeOverTree(Inverse(result, approximate));
    return result;
  }
  if (p == 0) {
    Tree* result = Integer::Push(NumberOfRows(m));
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
  Tree* result = Power(m, p / 2);
  result->moveTreeOverTree(Multiplication(result, result, approximate));
  if (p % 2 == 1) {
    result->moveTreeOverTree(Multiplication(m, result, approximate));
  }
  return result;
}

bool Matrix::SimplifySwitch(Tree* u) {
  // Dim is handled in Simplification::SimplifySwitch
  assert(u->isAMatrixOrContainsMatricesAsChildren() && !u->isDim());
  Tree* child = u->child(0);
  if (!child->isMatrix()) {
    if (u->isIdentity()) {
      u->moveTreeOverTree(Identity(child));
      return true;
    }
    return false;
  }
  switch (u->type()) {
    case BlockType::Cross:
    case BlockType::Dot: {
      Tree* child2 = child->nextTree();
      if (!u->child(1)->isMatrix()) {
        return false;
      }
      u->moveTreeOverTree(
          (u->isCross() ? Vector::Cross : Vector::Dot)(child, child2));
      return true;
    }
    case BlockType::PowerMatrix: {
      Tree* index = child->nextTree();
      if (!Integer::Is<int>(index)) {
        // TODO: Raise to rely on approximation.
        return false;
      }
      u->moveTreeOverTree(Power(child, Integer::Handler(index).to<int>()));
      return true;
    }
    case BlockType::Inverse:
      u->moveTreeOverTree(Inverse(child));
      return true;
    case BlockType::Ref:
      RowCanonize(child, false);
      u->removeNode();
      return true;
    case BlockType::Rref:
      RowCanonize(child, true);
      u->removeNode();
      return true;
    case BlockType::Trace:
      u->moveTreeOverTree(Trace(child));
      return true;
    case BlockType::Transpose:
      u->moveTreeOverTree(Transpose(child));
      return true;
    case BlockType::Det: {
      Tree* determinant;
      RowCanonize(child, true, &determinant);
      u->moveTreeOverTree(determinant);
      return true;
    }
    case BlockType::Norm:
      u->moveTreeOverTree(Vector::Norm(child));
      return true;
    default:
      // Remaining types have been handled beforehand.
      assert(false);
  }
  return false;
}

}  // namespace PoincareJ
