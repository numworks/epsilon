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

Matrix Matrix::rowCanonize(const ReductionContext& reductionContext,
                           Expression* determinant, bool reduced) {
  // The matrix children have to be reduced to be able to spot 0
  deepReduceChildren(reductionContext);

  Multiplication det = Multiplication::Builder();

  int m = numberOfRows();
  int n = numberOfColumns();

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
      float pivot =
          AbsoluteValue::Builder(matrixChild(iPivot_temp, k).clone())
              .approximateToScalar<float>(reductionContext.context(),
                                          reductionContext.complexFormat(),
                                          reductionContext.angleUnit(), true);
      // Handle very low pivots
      if (pivot == 0.0f &&
          matrixChild(iPivot_temp, k).isNull(reductionContext.context()) !=
              TrinaryBoolean::True) {
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
    if (matrixChild(iPivot, k).isNull(reductionContext.context()) ==
        TrinaryBoolean::True) {
      // No non-null coefficient in this column, skip
      k++;
      if (determinant) {
        // Update determinant: det *= 0
        det.addChildAtIndexInPlace(Rational::Builder(0), det.numberOfChildren(),
                                   det.numberOfChildren());
      }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          swapChildrenInPlace(iPivot * n + col, h * n + col);
        }
        if (determinant) {
          // Update determinant: det *= -1
          det.addChildAtIndexInPlace(Rational::Builder(-1),
                                     det.numberOfChildren(),
                                     det.numberOfChildren());
        }
      }
      // Set to 1 M[h][k] by linear combination
      Expression divisor = matrixChild(h, k);
      // Update determinant: det *= divisor
      if (determinant) {
        det.addChildAtIndexInPlace(divisor.clone(), det.numberOfChildren(),
                                   det.numberOfChildren());
      }
      for (int j = k + 1; j < n; j++) {
        Expression opHJ = matrixChild(h, j);
        Expression newOpHJ = Division::Builder(opHJ, divisor.clone());
        replaceChildAtIndexInPlace(h * n + j, newOpHJ);
        newOpHJ = newOpHJ.shallowReduce(reductionContext);
        if (reductionContext.target() == ReductionTarget::SystemForAnalysis &&
            newOpHJ.type() == ExpressionNode::Type::Dependency) {
          Expression e = newOpHJ.childAtIndex(0);
          newOpHJ.replaceChildAtIndexWithGhostInPlace(0);
          newOpHJ.replaceWithInPlace(e);
        }
      }
      replaceChildInPlace(divisor, Rational::Builder(1));

      int l = reduced ? 0 : h + 1;
      /* Set to 0 all M[i][j] i != h, j > k by linear combination. If a
       * non-reduced form is computed (ref), only rows below the pivot are
       * reduced, i > h as well */
      for (int i = l; i < m; i++) {
        if (i == h) {
          continue;
        }
        Expression factor = matrixChild(i, k);
        for (int j = k + 1; j < n; j++) {
          Expression opIJ = matrixChild(i, j);
          Expression newOpIJ = Subtraction::Builder(
              opIJ, Multiplication::Builder(matrixChild(h, j).clone(),
                                            factor.clone()));
          replaceChildAtIndexInPlace(i * n + j, newOpIJ);
          newOpIJ.childAtIndex(1).shallowReduce(reductionContext);
          newOpIJ = newOpIJ.shallowReduce(reductionContext);
          if (reductionContext.target() == ReductionTarget::SystemForAnalysis &&
              newOpIJ.type() == ExpressionNode::Type::Dependency) {
            Expression e = newOpIJ.childAtIndex(0);
            newOpIJ.replaceChildAtIndexWithGhostInPlace(0);
            newOpIJ.replaceWithInPlace(e);
          }
        }
        replaceChildAtIndexInPlace(i * n + k, Rational::Builder(0));
      }
      h++;
      k++;
    }
  }
  if (determinant) {
    *determinant = det;
  }
  return *this;
}

}  // namespace PoincareJ
