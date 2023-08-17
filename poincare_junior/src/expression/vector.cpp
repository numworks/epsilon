#include "vector.h"

namespace PoincareJ {

Expression Matrix::norm(const ReductionContext &reductionContext) const {
  // Norm is defined on vectors only
  assert(vectorType() != Array::VectorType::None);
  Addition sum = Addition::Builder();
  int childrenNumber = numberOfChildren();
  for (int j = 0; j < childrenNumber; j++) {
    Expression absValue = AbsoluteValue::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j).clone());
    Expression squaredAbsValue = Power::Builder(absValue, Rational::Builder(2));
    absValue.shallowReduce(reductionContext);
    sum.addChildAtIndexInPlace(squaredAbsValue, sum.numberOfChildren(),
                               sum.numberOfChildren());
    squaredAbsValue.shallowReduce(reductionContext);
  }
  Expression result = SquareRoot::Builder(sum);
  sum.shallowReduce(reductionContext);
  return result;
}

Expression Matrix::dot(Matrix *b,
                       const ReductionContext &reductionContext) const {
  // Dot product is defined between two vectors of same size and type
  assert(vectorType() != Array::VectorType::None &&
         vectorType() == b->vectorType() &&
         numberOfChildren() == b->numberOfChildren());
  Addition sum = Addition::Builder();
  int childrenNumber = numberOfChildren();
  for (int j = 0; j < childrenNumber; j++) {
    Expression product = Multiplication::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j).clone(),
        const_cast<Matrix *>(b)->childAtIndex(j).clone());
    sum.addChildAtIndexInPlace(product, sum.numberOfChildren(),
                               sum.numberOfChildren());
    product.shallowReduce(reductionContext);
  }
  return std::move(sum);
}

Matrix Matrix::cross(Matrix *b,
                     const ReductionContext &reductionContext) const {
  // Cross product is defined between two vectors of size 3 and of same type.
  assert(vectorType() != Array::VectorType::None &&
         vectorType() == b->vectorType() && numberOfChildren() == 3 &&
         b->numberOfChildren() == 3);
  Matrix matrix = Matrix::Builder();
  for (int j = 0; j < 3; j++) {
    int j1 = (j + 1) % 3;
    int j2 = (j + 2) % 3;
    Expression a1b2 = Multiplication::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j1).clone(),
        const_cast<Matrix *>(b)->childAtIndex(j2).clone());
    Expression a2b1 = Multiplication::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j2).clone(),
        const_cast<Matrix *>(b)->childAtIndex(j1).clone());
    Expression difference = Subtraction::Builder(a1b2, a2b1);
    a1b2.shallowReduce(reductionContext);
    a2b1.shallowReduce(reductionContext);
    matrix.addChildAtIndexInPlace(difference, matrix.numberOfChildren(),
                                  matrix.numberOfChildren());
    difference.shallowReduce(reductionContext);
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return matrix;
}

}  // namespace PoincareJ
