extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include <poincare/subtraction.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/opposite.h>
#include <poincare/complex_matrix.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"

namespace Poincare {

Expression::Type Subtraction::type() const {
  return Expression::Type::Subtraction;
}

Expression * Subtraction::clone() const {
  return new Subtraction(m_operands, true);
}

template<typename T>
Complex<T> Subtraction::compute(const Complex<T> c, const Complex<T> d) {
  return Complex<T>::Cartesian(c.a()-d.a(), c.b() - d.b());
}

Expression * Subtraction::shallowSimplify(Context& context, AngleUnit angleUnit) {
  Multiplication * m = new Multiplication(new Rational(Integer(-1)), operand(1), false);
  Addition * a = new Addition(operand(0), m, false);
  m->shallowSimplify(context, angleUnit);
  detachOperands();
  replaceWith(a, true);
  return a->shallowSimplify(context, angleUnit);
}

template<typename T> Evaluation<T> * Subtraction::computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * m) {
  Evaluation<T> * opposite = computeOnMatrixAndComplex(m, c);
  Complex<T> * operands = new Complex<T>[opposite->numberOfRows() * opposite->numberOfColumns()];
  for (int i = 0; i < opposite->numberOfOperands(); i++) {
    Complex<T> entry = *(opposite->complexOperand(i));
    operands[i] = Complex<T>::Cartesian(-entry.a(), -entry.b());
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  delete opposite;
  return result;
}

}
