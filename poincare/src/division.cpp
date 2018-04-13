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
#include "layout/fraction_layout.h"
#include <cmath>

namespace Poincare {

Expression::Type Division::type() const {
  return Type::Division;
}

Expression * Division::clone() const {
  return new Division(m_operands, true);
}

int Division::polynomialDegree(char symbolName) const {
  if (operand(1)->polynomialDegree(symbolName) != 0) {
    return -1;
  }
  return operand(0)->polynomialDegree(symbolName);
}

bool Division::needParenthesisWithParent(const Expression * e) const {
  Type types[] = {Type::Division, Type::Power, Type::Factorial};
  return e->isOfType(types, 3);
}

Expression * Division::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Power * p = new Power(operand(1), new Rational(-1), false);
  Multiplication * m = new Multiplication(operand(0), p, false);
  detachOperands();
  p->shallowReduce(context, angleUnit);
  replaceWith(m, true);
  return m->shallowReduce(context, angleUnit);
}

template<typename T>
std::complex<T> Division::compute(const std::complex<T> c, const std::complex<T> d) {
  return c/d;
}

ExpressionLayout * Division::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  const Expression * numerator = operand(0)->type() == Type::Parenthesis ? operand(0)->operand(0) : operand(0);
  const Expression * denominator = operand(1)->type() == Type::Parenthesis ? operand(1)->operand(0) : operand(1);
  return new FractionLayout(numerator->createLayout(floatDisplayMode, complexFormat), denominator->createLayout(floatDisplayMode, complexFormat), false);
}

template<typename T> MatrixComplex<T> Division::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n) {
  MatrixComplex<T> inverse = n.createInverse();
  MatrixComplex<T> result = Multiplication::computeOnComplexAndMatrix<T>(c, inverse);
  return result;
}

template<typename T> MatrixComplex<T> Division::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
  if (m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> inverse = n.createInverse();
  return Multiplication::computeOnMatrices<T>(m, inverse);
}

}
