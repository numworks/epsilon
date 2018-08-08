extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}

#include <poincare/division.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/tangent.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <cmath>
#include <poincare/fraction_layout_node.h>

namespace Poincare {

int Division::polynomialDegree(char symbolName) const {
  if (childAtIndex(1)->polynomialDegree(symbolName) != 0) {
    return -1;
  }
  return childAtIndex(0)->polynomialDegree(symbolName);
}

bool Division::needsParenthesesWithParent(const SerializationHelperInterface * parent) const {
  Type types[] = {Type::Division, Type::Power, Type::Factorial};
  return static_cast<ExpressionNode *>(parent)->isOfType(types, 3);
}

Expression Division::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  return Division(this).shallowReduce(context, angleUnit);
}

LayoutRef Division::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  const Expression * numerator = operand(0)->type() == Type::Parenthesis ? operand(0)->operand(0) : operand(0);
  const Expression * denominator = operand(1)->type() == Type::Parenthesis ? operand(1)->operand(0) : operand(1);
  return FractionLayoutRef(numerator->createLayout(floatDisplayMode, numberOfSignificantDigits), denominator->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

Complex<T> Division::compute(const std::complex<T> c, const std::complex<T> d) {
  return Complex<T>(c/d);
}

template<typename T> MatrixComplex<T> Division::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n) {
  MatrixComplex<T> * inverse = n.createInverse();
  if (inverse == nullptr) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> result = Multiplication::computeOnComplexAndMatrix<T>(c, *inverse);
  delete inverse;
  return result;
}

template<typename T> MatrixComplex<T> Division::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
  if (m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> * inverse = n.createInverse();
  if (inverse == nullptr) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> result = Multiplication::computeOnMatrices<T>(m, *inverse);
  delete inverse;
  return result;
}

// Division
Expression Division::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression result = Expression::shallowReduce(context, angleUnit);
#if 0
  // TODO: handle isFailed or isUndefined
  if (result.hasFailure()) {
    return result;
  }
#endif
  Power p = Power(operand(1), Rational(-1));
  p = p.shallowReduce(context, angleUnit); // Imagine Division(2,1). p would be 1^(-1) which can be simplified
  Multiplication m = Multiplication(operand(0), p);
  return m.shallowReduce(context, angleUnit);
}


}
