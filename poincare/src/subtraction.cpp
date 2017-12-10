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
  static const Type types[] = {Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return e->isOfType(types, 6);
}

template<typename T>
Complex<T> Subtraction::compute(const Complex<T> & c, const Complex<T> & d) {
  return Complex<T>::Cartesian(c.a()-d.a(), c.b() - d.b());
}

template<typename T> Matrix * Subtraction::computeOnComplexAndMatrix(const Complex<T> * c, const Matrix * m) {
  Matrix * opposite = computeOnMatrixAndComplex(m, c);
  if (opposite == nullptr) {
    return nullptr;
  }
  Expression ** operands = new Expression * [opposite->numberOfRows() * opposite->numberOfColumns()];
  for (int i = 0; i < opposite->numberOfOperands(); i++) {
    const Complex<T> * entry = static_cast<const Complex<T> *>(opposite->operand(i));
    operands[i] = new Complex<T>(Complex<T>::Cartesian(-entry->a(), -entry->b()));
  }
  Matrix * result = new Matrix(operands, m->numberOfRows(), m->numberOfColumns(), false);
  delete[] operands;
  delete opposite;
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
