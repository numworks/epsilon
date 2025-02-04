#include <assert.h>
#include <float.h>
#include <ion/circuit_breaker.h>
#include <omg/float.h>
#include <poincare/layout.h>
#include <poincare/old/absolute_value.h>
#include <poincare/old/addition.h>
#include <poincare/old/division.h>
#include <poincare/old/exception_checkpoint.h>
#include <poincare/old/matrix.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <cmath>
#include <utility>

namespace Poincare {

OExpression MatrixNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return OMatrix(this).shallowReduce(reductionContext);
}

size_t MatrixNode::serialize(char *buffer, size_t bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }
  size_t currentChar = SerializationHelper::CodePoint(buffer, bufferSize, '[');
  if (currentChar >= bufferSize - 1) {
    return currentChar;
  }
  for (int i = 0; i < m_numberOfRows; i++) {
    currentChar += SerializationHelper::CodePoint(
        buffer + currentChar, bufferSize - currentChar, '[');
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
    currentChar +=
        childAtIndex(i * m_numberOfColumns)
            ->serialize(buffer + currentChar, bufferSize - currentChar,
                        floatDisplayMode, numberOfSignificantDigits);
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
    for (int j = 1; j < m_numberOfColumns; j++) {
      currentChar += SerializationHelper::CodePoint(
          buffer + currentChar, bufferSize - currentChar, ',');
      if (currentChar >= bufferSize - 1) {
        return currentChar;
      }
      currentChar +=
          childAtIndex(i * m_numberOfColumns + j)
              ->serialize(buffer + currentChar, bufferSize - currentChar,
                          floatDisplayMode, numberOfSignificantDigits);
      if (currentChar >= bufferSize - 1) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
    currentChar += SerializationHelper::CodePoint(
        buffer + currentChar, bufferSize - currentChar, ']');
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
  }
  currentChar += SerializationHelper::CodePoint(buffer + currentChar,
                                                bufferSize - currentChar, ']');
  return std::min(currentChar, bufferSize - 1);
}

// MATRIX

void OMatrix::setDimensions(int rows, int columns) {
  assert(rows * columns == numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

void OMatrix::addChildrenAsRowInPlace(PoolHandle t, int i) {
  int previousNumberOfColumns = numberOfColumns();
  if (previousNumberOfColumns > 0) {
    assert(t.numberOfChildren() == numberOfColumns());
  }
  int previousNumberOfRows = numberOfRows();
  int childrenNumber = t.numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    addChildAtIndexInPlace(t.childAtIndex(i), numberOfChildren(),
                           numberOfChildren());
  }
  setDimensions(previousNumberOfRows + 1, previousNumberOfColumns == 0
                                              ? t.numberOfChildren()
                                              : previousNumberOfColumns);
}

int OMatrix::rank(Context *context, bool forceCanonization) {
  assert(!recursivelyMatches(OExpression::IsUninitialized, context));
  if (recursivelyMatches(OExpression::IsUndefined, context)) {
    return -1;
  }
  OExpression m;
  ReductionContext systemReductionContext =
      ReductionContext::DefaultReductionContextForAnalysis(context);

  bool canonizationSuccess = true;
  {
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      OMatrix cannonizedM = clone().convert<OMatrix>();
      m = cannonizedM.rowCanonize(systemReductionContext, &canonizationSuccess,
                                  nullptr, true, forceCanonization);
    } else {
      /* rowCanonize can create expression that are too big for the pool.
       * If it's the case, compute the rank with approximated values. */
      OExpression mApproximation =
          approximate<double>(ApproximationContext(systemReductionContext));
      if (mApproximation.otype() != ExpressionNode::Type::OMatrix) {
        /* The approximation was able to conclude that a coefficient is undef
         * while the reduction could not. */
        return -1;
      }
      m = static_cast<OMatrix &>(mApproximation)
              .rowCanonize(systemReductionContext, &canonizationSuccess,
                           nullptr, true, forceCanonization);
    }
  }

  if (!canonizationSuccess ||
      m.recursivelyMatches(OExpression::IsUndefined, context)) {
    return -1;
  }

  assert(m.otype() == ExpressionNode::Type::OMatrix);
  OMatrix cannonizedMatrix = static_cast<OMatrix &>(m);
  *this = cannonizedMatrix;

  int rank = cannonizedMatrix.numberOfRows();
  int i = rank - 1;
  while (i >= 0) {
    int j = cannonizedMatrix.numberOfColumns() - 1;
    // TODO: Handle OMG::Troolean::Unknown. See rowCanonize comment
    while (j >= i && cannonizedMatrix.matrixChild(i, j).isNull(context) ==
                         OMG::Troolean::True) {
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

OExpression OMatrix::createTrace() {
  assert(numberOfRows() == numberOfColumns());
  int n = numberOfRows();
  Addition a = Addition::Builder();
  for (int i = 0; i < n; i++) {
    a.addChildAtIndexInPlace(matrixChild(i, i).clone(), i, i);
  }
  return std::move(a);
}

bool OMatrix::isCanonizable(const ReductionContext &reductionContext) {
  ApproximationContext approximationContext(reductionContext);
  int m = numberOfRows();
  int n = numberOfColumns();
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      if (std::isnan(
              AbsoluteValue::Builder(matrixChild(i, j).clone())
                  .approximateToRealScalar<float>(approximationContext))) {
        return false;
      }
    }
  }
  return true;
}

OMatrix OMatrix::rowCanonize(const ReductionContext &reductionContext,
                             bool *canonizationSuccess,
                             OExpression *determinant, bool reduced,
                             bool forceCanonization) {
  assert(canonizationSuccess);
  *canonizationSuccess = true;

  // The matrix children have to be reduced to be able to spot 0
  deepReduceChildren(reductionContext);

  if (!forceCanonization && !isCanonizable(reductionContext)) {
    *canonizationSuccess = false;
    return *this;
  }

  Multiplication det = Multiplication::Builder();

  int m = numberOfRows();
  int n = numberOfColumns();

  int h = 0;  // row pivot
  int k = 0;  // column pivot

  ApproximationContext approximationContext(reductionContext);

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
      float pivot = AbsoluteValue::Builder(matrixChild(iPivot_temp, k).clone())
                        .approximateToRealScalar<float>(approximationContext);
      // Handle very low pivots
      if (pivot == 0.0f &&
          matrixChild(iPivot_temp, k).isNull(reductionContext.context()) !=
              OMG::Troolean::True) {
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
    /* TODO: Handle isNull == OMG::Troolean::Unknown : rowCanonize will
     * output a mathematically wrong result (and divide expressions by a null
     * expression) if expression is actually null. For examples,
     * 1-cos(x)^2-sin(x)^2 would be mishandled. */
    if (matrixChild(iPivot, k).isNull(reductionContext.context()) ==
        OMG::Troolean::True) {
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
      OExpression divisor = matrixChild(h, k);
      // Update determinant: det *= divisor
      if (determinant) {
        det.addChildAtIndexInPlace(divisor.clone(), det.numberOfChildren(),
                                   det.numberOfChildren());
      }
      for (int j = k + 1; j < n; j++) {
        OExpression opHJ = matrixChild(h, j);
        OExpression newOpHJ = Division::Builder(opHJ, divisor.clone());
        replaceChildAtIndexInPlace(h * n + j, newOpHJ);
        newOpHJ = newOpHJ.shallowReduce(reductionContext);
        if (reductionContext.target() == ReductionTarget::SystemForAnalysis &&
            newOpHJ.otype() == ExpressionNode::Type::Dependency) {
          OExpression e = newOpHJ.childAtIndex(0);
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
        OExpression factor = matrixChild(i, k);
        for (int j = k + 1; j < n; j++) {
          OExpression opIJ = matrixChild(i, j);
          OExpression newOpIJ = Subtraction::Builder(
              opIJ, Multiplication::Builder(matrixChild(h, j).clone(),
                                            factor.clone()));
          replaceChildAtIndexInPlace(i * n + j, newOpIJ);
          newOpIJ.childAtIndex(1).shallowReduce(reductionContext);
          newOpIJ = newOpIJ.shallowReduce(reductionContext);
          if (reductionContext.target() == ReductionTarget::SystemForAnalysis &&
              newOpIJ.otype() == ExpressionNode::Type::Dependency) {
            OExpression e = newOpIJ.childAtIndex(0);
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

OMatrix OMatrix::CreateIdentity(int dim) {
  OMatrix matrix = OMatrix::Builder();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      matrix.addChildAtIndexInPlace(
          i == j ? Rational::Builder(1) : Rational::Builder(0), i * dim + j,
          i * dim + j);
    }
  }
  matrix.setDimensions(dim, dim);
  return matrix;
}

OMatrix OMatrix::createTranspose() const {
  OMatrix matrix = OMatrix::Builder();
  for (int j = 0; j < numberOfColumns(); j++) {
    for (int i = 0; i < numberOfRows(); i++) {
      matrix.addChildAtIndexInPlace(
          const_cast<OMatrix *>(this)->matrixChild(i, j).clone(),
          matrix.numberOfChildren(), matrix.numberOfChildren());
    }
  }
  // Intentionally swapping dimensions for transpose
  matrix.setDimensions(numberOfColumns(), numberOfRows());
  return matrix;
}

OExpression OMatrix::createRef(const ReductionContext &reductionContext,
                               bool *couldComputeRef, bool reduced) const {
  // Compute OMatrix Row Echelon Form
  /* If the matrix is too big, the rowCanonization might not be computed exactly
   * because of a pool allocation error, but we might still be able to compute
   * it approximately. We thus encapsulate the ref creation in an exception
   * checkpoint.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there is no Store
   * in the matrix. */
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    /* We clone the current matrix to extract its children later. We can't clone
     * its children directly. Indeed, the current matrix node (this->node()) is
     * located before the exception checkpoint. In order to clone its chidlren,
     * we would temporary increase the reference counter of each child (also
     * located before the checkpoint). If an exception is raised before
     * destroying the child handle, its reference counter would be off by one
     * after the long jump. */
    OMatrix result = clone().convert<OMatrix>();
    /* Reduced row echelon form is also called row canonical form. To compute
     * the row echelon form (non reduced one), fewer steps are required. */
    result =
        result.rowCanonize(reductionContext, couldComputeRef, nullptr, reduced);
    return std::move(result);
  } else {
    *couldComputeRef = false;
    return OExpression();
  }
}

OExpression OMatrix::createInverse(const ReductionContext &reductionContext,
                                   bool *couldComputeInverse) const {
  int dim = numberOfRows();
  if (dim != numberOfColumns()) {
    *couldComputeInverse = true;
    return Undefined::Builder();
  }
  OExpression result =
      computeInverseOrDeterminant(false, reductionContext, couldComputeInverse);
  assert(!(*couldComputeInverse) || !result.isUninitialized());
  return result;
}

OExpression OMatrix::determinant(const ReductionContext &reductionContext,
                                 bool *couldComputeDeterminant, bool inPlace) {
  // Determinant must be called on a reduced matrix only.
  *couldComputeDeterminant = true;
  OMatrix m = inPlace ? *this : clone().convert<OMatrix>();
  int dim = m.numberOfRows();
  if (dim != m.numberOfColumns()) {
    // Determinant is for square matrices
    return Undefined::Builder();
  }
  // Explicit formulas for dimensions from 1 to 3
  if (dim == 1) {
    // Determinant of [[a]] is a
    return m.childAtIndex(0);
  }
  if (dim == 2) {
    /*                |a b|
     * Determinant of |c d| is ad-bc   */
    Multiplication ad =
        Multiplication::Builder(m.matrixChild(0, 0), m.matrixChild(1, 1));
    Multiplication bc =
        Multiplication::Builder(m.matrixChild(0, 1), m.matrixChild(1, 0));
    OExpression result = Subtraction::Builder(ad, bc);
    ad.shallowReduce(reductionContext);
    bc.shallowReduce(reductionContext);
    return result;
  }
  if (dim == 3) {
    /*                |a b c|
     * Determinant of |d e f| is aei+bfg+cdh-ceg-bdi-afh
     *                |g h i|                             */
    OExpression a = m.matrixChild(0, 0);
    OExpression b = m.matrixChild(0, 1);
    OExpression c = m.matrixChild(0, 2);
    OExpression d = m.matrixChild(1, 0);
    OExpression e = m.matrixChild(1, 1);
    OExpression f = m.matrixChild(1, 2);
    OExpression g = m.matrixChild(2, 0);
    OExpression h = m.matrixChild(2, 1);
    OExpression i = m.matrixChild(2, 2);
    Tuple children = {Multiplication::Builder(a.clone(), e.clone(), i.clone()),
                      Multiplication::Builder(b.clone(), f.clone(), g.clone()),
                      Multiplication::Builder(c.clone(), d.clone(), h.clone()),
                      Multiplication::Builder(Rational::Builder(-1), c, e, g),
                      Multiplication::Builder(Rational::Builder(-1), b, d, i),
                      Multiplication::Builder(Rational::Builder(-1), a, f, h)};
    OExpression result = Addition::Builder(children);
    for (OExpression child : children) {
      child.shallowReduce(reductionContext);
    }
    return result;
  }

  // Dimension >= 4
  OExpression result = computeInverseOrDeterminant(true, reductionContext,
                                                   couldComputeDeterminant);
  assert(!(*couldComputeDeterminant) || !result.isUninitialized());
  return result;
}

OExpression OMatrix::norm(const ReductionContext &reductionContext) const {
  // Norm is defined on vectors only
  assert(isVector());
  Addition sum = Addition::Builder();
  int childrenNumber = numberOfChildren();
  for (int j = 0; j < childrenNumber; j++) {
    OExpression absValue = AbsoluteValue::Builder(
        const_cast<OMatrix *>(this)->childAtIndex(j).clone());
    OExpression squaredAbsValue =
        Power::Builder(absValue, Rational::Builder(2));
    absValue.shallowReduce(reductionContext);
    sum.addChildAtIndexInPlace(squaredAbsValue, sum.numberOfChildren(),
                               sum.numberOfChildren());
    squaredAbsValue.shallowReduce(reductionContext);
  }
  OExpression result = SquareRoot::Builder(sum);
  sum.shallowReduce(reductionContext);
  return result;
}

OExpression OMatrix::dot(OMatrix *b,
                         const ReductionContext &reductionContext) const {
  // Dot product is defined between two vectors of same size and type
  assert(isVector() && vectorType() == b->vectorType() &&
         numberOfChildren() == b->numberOfChildren());
  Addition sum = Addition::Builder();
  int childrenNumber = numberOfChildren();
  for (int j = 0; j < childrenNumber; j++) {
    OExpression product = Multiplication::Builder(
        const_cast<OMatrix *>(this)->childAtIndex(j).clone(),
        const_cast<OMatrix *>(b)->childAtIndex(j).clone());
    sum.addChildAtIndexInPlace(product, sum.numberOfChildren(),
                               sum.numberOfChildren());
    product.shallowReduce(reductionContext);
  }
  return std::move(sum);
}

OMatrix OMatrix::cross(OMatrix *b,
                       const ReductionContext &reductionContext) const {
  // Cross product is defined between two vectors of size 3 and of same type.
  assert(isVector() && vectorType() == b->vectorType() &&
         numberOfChildren() == 3 && b->numberOfChildren() == 3);
  OMatrix matrix = OMatrix::Builder();
  for (int j = 0; j < 3; j++) {
    int j1 = (j + 1) % 3;
    int j2 = (j + 2) % 3;
    OExpression a1b2 = Multiplication::Builder(
        const_cast<OMatrix *>(this)->childAtIndex(j1).clone(),
        const_cast<OMatrix *>(b)->childAtIndex(j2).clone());
    OExpression a2b1 = Multiplication::Builder(
        const_cast<OMatrix *>(this)->childAtIndex(j2).clone(),
        const_cast<OMatrix *>(b)->childAtIndex(j1).clone());
    OExpression difference = Subtraction::Builder(a1b2, a2b1);
    a1b2.shallowReduce(reductionContext);
    a2b1.shallowReduce(reductionContext);
    matrix.addChildAtIndexInPlace(difference, matrix.numberOfChildren(),
                                  matrix.numberOfChildren());
    difference.shallowReduce(reductionContext);
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return matrix;
}

OExpression OMatrix::shallowReduce(ReductionContext reductionContext) {
  if (node()->hasMatrixOrListChild(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }
  OExpression e =
      SimplificationHelper::bubbleUpDependencies(*this, reductionContext);
  if (!e.isUninitialized()) {
    return e;
  }
  return *this;
}

OExpression OMatrix::computeInverseOrDeterminant(
    bool computeDeterminant, const ReductionContext &reductionContext,
    bool *couldCompute) const {
  assert(numberOfRows() == numberOfColumns());
  int dim = numberOfRows();
  /* If the matrix is too big, the rowCanonization might not be computed exactly
   * because of a pool allocation error, but we might still be able to compute
   * it approximately. We thus encapsulate the inverse/determinant creation in
   * an exception checkpoint.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there is no Store
   * in the matrix. */
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    /* We clone the current matrix to extract its children later. We can't clone
     * its children directly. Indeed, the current matrix node (this->node()) is
     * located before the exception checkpoint. In order to clone its chidlren,
     * we would temporary increase the reference counter of each child (also
     * located before the checkpoint). If an exception is raised before
     * destroying the child handle, its reference counter would be off by one
     * after the long jump. */
    OMatrix cl = clone().convert<OMatrix>();
    *couldCompute = true;
    /* Create the matrix (A|I) with A is the input matrix and I the dim
     * identity matrix */
    OMatrix matrixAI = OMatrix::Builder();
    for (int i = 0; i < dim; i++) {
      for (int j = 0; j < dim; j++) {
        OExpression mChildIJ = cl.matrixChild(i, j);
        matrixAI.addChildAtIndexInPlace(mChildIJ, i * 2 * dim + j,
                                        i * 2 * dim + j);
      }
      for (int j = dim; j < 2 * dim; j++) {
        matrixAI.addChildAtIndexInPlace(
            j - dim == i ? Rational::Builder(1) : Rational::Builder(0),
            i * 2 * dim + j, i * 2 * dim + j);
      }
    }
    matrixAI.setDimensions(dim, 2 * dim);
    OExpression det;
    bool canonizationSuccess = true;
    matrixAI =
        matrixAI.rowCanonize(reductionContext, &canonizationSuccess, &det);
    if (!canonizationSuccess) {
      *couldCompute = false;
      return Undefined::Builder();
    }
    if (computeDeterminant) {
      return det;
    }
    // Check inversibility
    for (int i = 0; i < dim; i++) {
      if (!matrixAI.matrixChild(i, i).isRationalOne()) {
        return Undefined::Builder();
      }
    }
    OMatrix inverse = OMatrix::Builder();
    for (int i = 0; i < dim; i++) {
      for (int j = 0; j < dim; j++) {
        // We can steal matrixAI's children
        inverse.addChildAtIndexInPlace(matrixAI.matrixChild(i, j + dim),
                                       i * dim + j, i * dim + j);
      }
    }
    inverse.setDimensions(dim, dim);
    return std::move(inverse);
  } else {
    *couldCompute = false;
    return OExpression();
  }
}

}  // namespace Poincare
