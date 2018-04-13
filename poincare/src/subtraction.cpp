#include <poincare/subtraction.h>
#include <poincare/addition.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

Expression::Type Subtraction::type() const {
  return Expression::Type::Subtraction;
}

Expression * Subtraction::clone() const {
  return new Subtraction(m_operands, true);
}

int Subtraction::polynomialDegree(char symbolName) const {
  int degree = 0;
  for (int i = 0; i < numberOfOperands(); i++) {
    int d = operand(i)->polynomialDegree(symbolName);
    if (d < 0) {
      return -1;
    }
    degree = d > degree ? d : degree;
  }
  return degree;
}

/* Layout */

bool Subtraction::needParenthesisWithParent(const Expression * e) const {
  Type types[] = {Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return e->isOfType(types, 6);
}

template<typename T>
std::complex<T> Subtraction::compute(const std::complex<T> c, const std::complex<T> d) {
  return c - d;
}

template<typename T> MatrixComplex<T> Subtraction::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
  MatrixComplex<T> opposite = computeOnMatrixAndComplex(m, c);
  std::complex<T> * operands = new std::complex<T> [opposite.numberOfComplexOperands()];
  for (int i = 0; i < opposite.numberOfComplexOperands(); i++) {
    operands[i] = -opposite.complexOperand(i);
  }
  MatrixComplex<T> result = MatrixComplex<T>(operands, opposite.numberOfRows(), opposite.numberOfColumns());
  delete[] operands;
  return result;
}

Expression * Subtraction::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Multiplication * m = new Multiplication(new Rational(-1), operand(1), false);
  Addition * a = new Addition(operand(0), m, false);
  detachOperands();
  m->shallowReduce(context, angleUnit);
  replaceWith(a, true);
  return a->shallowReduce(context, angleUnit);
}

}
