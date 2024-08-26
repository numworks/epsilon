#include "matrix.h"

#include <float.h>
#include <limits.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "approximation.h"
#include "float_helper.h"
#include "integer.h"
#include "k_tree.h"
#include "number.h"
#include "systematic_reduction.h"
#include "vector.h"

namespace Poincare::Internal {

Tree* Matrix::Zero(MatrixDimension d) {
  Tree* result = SharedTreeStack->pushMatrix(d.rows, d.cols);
  for (int i = 0; i < d.rows * d.cols; i++) {
    (0_e)->cloneTree();
  }
  return result;
}

Tree* Matrix::Identity(const Tree* n) {
  assert(n->isNumber());
  if (Integer::Handler(n).numberOfDigits() > 1) {
    return KUndefUnhandled->cloneTree();
  }
  uint8_t nb = *Integer::Handler(n).digits();
  Tree* result = SharedTreeStack->pushMatrix(nb, nb);
  for (int i = 0; i < nb - 1; i++) {
    (1_e)->cloneTree();
    // cloning n zeros is indeed a memset(0)
    for (int j = 0; j < nb; j++) {
      (0_e)->cloneTree();
    }
  }
  (1_e)->cloneTree();
  return result;
}

Tree* Matrix::Trace(const Tree* matrix) {
  int n = NumberOfRows(matrix);
  assert(n == NumberOfColumns(matrix));
  Tree* result = SharedTreeStack->pushAdd(n);
  const Tree* child = matrix->child(0);
  for (int i = 0; i < n - 1; i++) {
    child->cloneTree();
    for (int j = 0; j < n + 1; j++) {
      child = child->nextTree();
    }
  }
  child->cloneTree();
  SystematicReduction::ShallowReduce(result);
  return result;
}

Tree* Matrix::Transpose(const Tree* matrix) {
  uint8_t rows = NumberOfRows(matrix);
  uint8_t cols = NumberOfColumns(matrix);
  if (rows == 1 || cols == 1) {
    Tree* result = matrix->cloneTree();
    SetNumberOfRows(result, cols);
    SetNumberOfColumns(result, rows);
    return result;
  }
  Tree* result = SharedTreeStack->pushMatrix(cols, rows);
  const Tree* rowsM[rows];
  const Tree* child = matrix->child(0);
  for (int row = 0; row < rows; row++) {
    rowsM[row] = child;
    for (int col = 0; col < cols; col++) {
      child = child->nextTree();
    }
  }
  for (int col = 0; col < cols; col++) {
    for (int row = 0; row < rows; row++) {
      rowsM[row]->cloneTree();
      rowsM[row] = rowsM[row]->nextTree();
    }
  }
  return result;
}

Tree* Matrix::Addition(const Tree* matrix1, const Tree* matrix2,
                       bool approximate) {
  // should be an assert after dimensional analysis
  assert(NumberOfRows(matrix1) == NumberOfRows(matrix2) &&
         NumberOfColumns(matrix1) == NumberOfColumns(matrix2));
  const Tree* childM1 = matrix1->child(0);
  const Tree* childM2 = matrix2->child(0);
  int n = matrix1->numberOfChildren();
  Tree* result = matrix1->cloneNode();
  for (int i = 0; i < n; i++) {
    Tree* child = KAdd.node<2>->cloneNode();
    childM1->cloneTree();
    childM2->cloneTree();
    if (approximate) {
      Approximation::ApproximateToComplexTree(child);
    } else {
      SystematicReduction::ShallowReduce(child);
    }
    childM1 = childM1->nextTree();
    childM2 = childM2->nextTree();
  }
  return result;
}

Tree* Matrix::ScalarMultiplication(const Tree* scalar, const Tree* matrix,
                                   bool approximate) {
  Tree* result = matrix->cloneNode();
  for (const Tree* child : matrix->children()) {
    Tree* mult = SharedTreeStack->pushMult(2);
    scalar->cloneTree();
    child->cloneTree();
    if (approximate) {
      Approximation::ApproximateToComplexTree(mult);
    } else {
      SystematicReduction::ShallowReduce(mult);
    }
  }
  return result;
}

Tree* Matrix::Multiplication(const Tree* matrix1, const Tree* matrix2,
                             bool approximate) {
  assert(NumberOfColumns(matrix1) == NumberOfRows(matrix2));
  uint8_t rows = NumberOfRows(matrix1);
  uint8_t internal = NumberOfColumns(matrix1);
  uint8_t cols = NumberOfColumns(matrix2);
  Tree* result = SharedTreeStack->pushMatrix(rows, cols);
  /* The complexity of the naive multiplication is n^3 by itself but if we do
   * not take care, indexing the children with child will add an n^2
   * factor. To avoid this, we keep track of each row of matrix2. */
  const Tree* rowsM2[internal];
  {
    // Initialize row pointers
    const Tree* childM2 = matrix2->child(0);
    for (int k = 0; k < internal; k++) {
      rowsM2[k] = childM2;
      for (int c = 0; c < cols; c++) {
        childM2 = childM2->nextTree();
      }
    }
  }
  const Tree* childM1Row0 = matrix1->child(0);
  const Tree* childM1RowK;
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      Tree* add = SharedTreeStack->pushAdd(internal);
      childM1RowK = childM1Row0;
      for (int k = 0; k < internal; k++) {
        Tree* mult = SharedTreeStack->pushMult(2);
        assert(childM1RowK == Child(matrix1, row, k));
        childM1RowK->cloneTree();
        childM1RowK = childM1RowK->nextTree();
        assert(rowsM2[k] == Child(matrix2, k, col));
        rowsM2[k]->cloneTree();
        rowsM2[k] = rowsM2[k]->nextTree();
        if (approximate) {
          Approximation::ApproximateToComplexTree(mult);
        } else {
          SystematicReduction::ShallowReduce(mult);
        }
      }
      if (approximate) {
        Approximation::ApproximateToComplexTree(add);
      } else {
        SystematicReduction::ShallowReduce(add);
      }
    }
    childM1Row0 = childM1RowK;
    // Since each row has moved cols times we can shift them to restore them
    for (int k = internal - 1; k > 0; k--) {
      rowsM2[k] = rowsM2[k - 1];
    }
    rowsM2[0] = matrix2->child(0);
  }
  return result;
}

bool Matrix::RowCanonize(Tree* matrix, bool reducedForm, Tree** determinant,
                         bool approximate) {
  // The matrix children have to be reduced to be able to spot 0
  assert(approximate || !SystematicReduction::DeepReduce(matrix));

  TreeRef det;
  if (determinant) {
    det = SharedTreeStack->pushMult(0);
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
      if (pivot == 0.0f &&
          !(pivotChild->isZero() ||
            (pivotChild->isFloat() && FloatHelper::FloatTo(pivotChild) == 0))) {
        pivot = FLT_MIN;
      }

      if (pivot > bestPivot) {
        // Update best pivot
        bestPivot = pivot;
        iPivot = iPivot_temp;
        if (reducedForm) {
          /* In reduced form, taking the first non null pivot is enough, and
           * more efficient. */
          break;
        }
      }
      iPivot_temp++;
    }
    /* TODO: Handle isNull == OMG::Troolean::Unknown : rowCanonize will
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
        NAry::AddChild(det, (0_e)->cloneTree());
      }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          Child(matrix, iPivot, col)->swapWithTree(Child(matrix, h, col));
        }
        if (determinant) {
          // Update determinant: det *= -1
          NAry::AddChild(det, (-1_e)->cloneTree());
        }
      }
      // Set to 1 M[h][k] by linear combination
      Tree* divisor = Child(matrix, h, k);
      if (determinant) {
        // Update determinant: det *= divisor
        NAry::AddChild(det, divisor->cloneTree());
      }
      Tree* opHJ = divisor;
      for (int j = k + 1; j < n; j++) {
        opHJ = opHJ->nextTree();
        if (approximate) {
          Tree* newOpHJ = PatternMatching::Create(KMult(KA, KPow(KB, -1_e)),
                                                  {.KA = opHJ, .KB = divisor});
          Approximation::ApproximateToComplexTree(newOpHJ);
          opHJ->moveTreeOverTree(newOpHJ);
        } else {
          opHJ->moveTreeOverTree(PatternMatching::CreateSimplify(
              KMult(KA, KPow(KB, -1_e)), {.KA = opHJ, .KB = divisor}));
        }
        // TODO_PCJ: Dependency
      }
      divisor->cloneTreeOverTree(1_e);

      int l = reducedForm ? 0 : h + 1;
      /* Set to 0 all M[i][j] i != h, j > k by linear combination. If a
       * non-reduced form is computed (ref), only rows below the pivot are
       * reduced, i > h as well */
      for (int i = l; i < m; i++) {
        if (i == h) {
          continue;
        }
        Tree* factor = Child(matrix, i, k);
        Tree* opIJ = factor;
        TreeRef opHJ = Child(matrix, h, k);  // opHJ may be after opIJ
        for (int j = k + 1; j < n; j++) {
          opIJ = opIJ->nextTree();
          opHJ = opHJ->nextTree();
          if (approximate) {
            Tree* newOpIJ =
                PatternMatching::Create(KAdd(KA, KMult(-1_e, KB, KC)),
                                        {.KA = opIJ, .KB = opHJ, .KC = factor});
            Approximation::ApproximateToComplexTree(newOpIJ);
            opIJ->moveTreeOverTree(newOpIJ);
          } else {
            opIJ->moveTreeOverTree(PatternMatching::CreateSimplify(
                KAdd(KA, KMult(-1_e, KB, KC)),
                {.KA = opIJ, .KB = opHJ, .KC = factor}));
          }
          // TODO_PCJ: Dependency
        }
        factor->cloneTreeOverTree(0_e);
      }
      h++;
      k++;
    }
  }
  if (determinant) {
    if (approximate) {
      Approximation::ApproximateToComplexTree(det);
    } else {
      SystematicReduction::ShallowReduce(det);
    }
    *determinant = det;
  }
  if (!approximate) {
    SystematicReduction::ShallowReduce(matrix);
  }
  return true;
}

int Matrix::Rank(const Tree* matrix) {
  Tree* copy = matrix->cloneTree();
  RowCanonize(copy);
  int rank = RankOfCanonized(copy);
  copy->removeTree();
  return rank;
}

int Matrix::CanonizeAndRank(Tree* matrix) {
  RowCanonize(matrix);
  return RankOfCanonized(matrix);
}

int Matrix::RankOfCanonized(const Tree* matrix) {
  int rank = NumberOfRows(matrix);
  int i = rank - 1;
  while (i >= 0) {
    int j = NumberOfColumns(matrix) - 1;
    // TODO: Handle OMG::Troolean::Unknown. See rowCanonize comment
    while (j >= i && Child(matrix, i, j)->isZero()) {
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

Tree* Matrix::Inverse(const Tree* matrix, bool approximate) {
  assert(NumberOfRows(matrix) == NumberOfColumns(matrix));
  int dim = NumberOfRows(matrix);
  /* Create the matrix (A|I) with A is the input matrix and I the dim
   * identity matrix */
  Tree* matrixAI = SharedTreeStack->pushMatrix(dim, dim * 2);
  const Tree* childIJ = matrix->child(0);
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      childIJ->cloneTree();
      childIJ = childIJ->nextTree();
    }
    for (int j = 0; j < dim; j++) {
      (i == j ? 1_e : 0_e)->cloneTree();
    }
  }
  // Compute the inverse
  RowCanonize(matrixAI, true, nullptr, approximate);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    if (!Child(matrixAI, i, i)->isOne()) {
      matrixAI->cloneTreeOverTree(KUndefUnhandled);
      return matrixAI;
    }
  }
  // Remove A from (A|I)
  Tree* child = matrixAI->child(0);
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

Tree* Matrix::Power(const Tree* matrix, int exponent, bool approximate) {
  assert(NumberOfRows(matrix) == NumberOfColumns(matrix));
  if (exponent < 0 && exponent != INT_MIN) {
    // -INT_MIN is not possible
    assert(-exponent > 0);
    Tree* result = Power(matrix, -exponent);
    // TODO is it worth to compute inverse first ?
    result->moveTreeOverTree(Inverse(result, approximate));
    return result;
  }
  assert(exponent >= 0 || exponent % 2 == 0);
  if (exponent == 0) {
    Tree* result = Integer::Push(NumberOfRows(matrix));
    result->moveTreeOverTree(Identity(result));
    return result;
  }
  if (exponent == 1) {
    return matrix->cloneTree();
  }
  if (exponent == 2) {
    return Multiplication(matrix, matrix);
  }
  // Quick exponentiation
  Tree* result = Power(matrix, exponent / 2);
  result->moveTreeOverTree(Multiplication(result, result, approximate));
  if (exponent % 2 == 1) {
    result->moveTreeOverTree(Multiplication(matrix, result, approximate));
  }
  return result;
}

bool Matrix::SystematicReduceMatrixOperation(Tree* e) {
  // Dim is handled in SystematicReduction::Switch
  assert(e->isAMatrixOrContainsMatricesAsChildren() && !e->isDim());
  Tree* child = e->child(0);
  if (!child->isMatrix() && !e->isIdentity()) {
    return false;
  }
  if (e->isRef() || e->isRref()) {
    RowCanonize(child, e->isRref());
    e->removeNode();
    return true;
  }
  Tree* result;
  switch (e->type()) {
    case Type::Cross:
    case Type::Dot: {
      Tree* child2 = child->nextTree();
      if (!e->child(1)->isMatrix()) {
        return false;
      }
      result = (e->isCross() ? Vector::Cross : Vector::Dot)(child, child2);
      break;
    }
    case Type::Det:
      RowCanonize(child, true, &result);
      break;
    case Type::Identity:
      result = Identity(child);
      break;
    case Type::Inverse:
      result = Inverse(child);
      break;
    case Type::Norm:
      result = Vector::Norm(child);
      break;
    case Type::PowMatrix: {
      Tree* index = child->nextTree();
      if (!Integer::Is<int>(index)) {
        // TODO: Raise to rely on approximation.
        return false;
      }
      result = Power(child, Integer::Handler(index).to<int>());
      break;
    }
    case Type::Trace:
      result = Trace(child);
      break;
    case Type::Transpose:
      result = Transpose(child);
      break;
    default:  // Remaining types should have been handled beforehand.
      assert(false);
      return false;
  }
  e->moveTreeOverTree(result);
  return true;
}

}  // namespace Poincare::Internal
