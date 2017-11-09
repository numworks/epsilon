extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include <poincare/subtraction.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/opposite.h>
#include <poincare/matrix.h>
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
