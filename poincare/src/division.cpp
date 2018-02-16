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
Complex<T> Division::compute(const Complex<T> c, const Complex<T> d) {
  /* We want to avoid multiplies in the middle of the calculation that could
   * overflow.
   * aa, ab, ba, bb, min, max = |d.a| <= |d.b| ? (c.a, c.b, -c.a, c.b, d.a, d.b)
   *    : (c.b, c.a, c.b, -c.a, d.b, d.a)
   * c    c.a+c.b*i   d.a-d.b*i   1/max    (c.a+c.b*i) * (d.a-d.b*i) / max
   * - == --------- * --------- * ----- == -------------------------------
   * d    d.a+d.b*i   d.a-d.b*i   1/max    (d.a+d.b*i) * (d.a-d.b*i) / max
   *      (c.a*d.a - c.a*d.b*i + c.b*i*d.a - c.b*i*d.b*i) / max
   *   == -----------------------------------------------------
   *      (d.a*d.a - d.a*d.b*i + d.b*i*d.a - d.b*i*d.b*i) / max
   *      (c.a*d.a - c.b*d.b*i^2 + c.b*d.b*i - c.a*d.a*i) / max
   *   == -----------------------------------------------------
   *                  (d.a*d.a - d.b*d.b*i^2) / max
   *      (c.a*d.a/max + c.b*d.b/max) + (c.b*d.b/max - c.a*d.a/max)*i
   *   == -----------------------------------------------------------
   *                         d.a^2/max + d.b^2/max
   *      aa*min/max + ab*max/max   bb*min/max + ba*max/max
   *   == ----------------------- + -----------------------*i
   *       min^2/max + max^2/max     min^2/max + max^2/max
   *       min/max*aa + ab     min/max*bb + ba
   *   == ----------------- + -----------------*i
   *      min/max*min + max   min/max*min + max
   * |min| <= |max| => |min/max| <= 1
   *                => |min/max*x| <= |x|
   *                => |min/max*x+y| <= |x|+|y|
   * So the calculation is guaranteed to not overflow until the last divides as
   * long as none of the input values have the representation's maximum exponent.
   * Plus, the method does not propagate any error on real inputs: temp = 0,
   * norm = d.a and then result = c.a/d.a. */
  T aa = c.a(), ab = c.b(), ba = -aa, bb = ab;
  T min = d.a(), max = d.b();
  if (std::fabs(max) < std::fabs(min)) {
    T temp = min;
    min = max;
    max = temp;
    temp = aa;
    aa = ab;
    ab = temp;
    temp = ba;
    ba = bb;
    bb = temp;
  }
  T temp = min/max;
  T norm = temp*min + max;
  return Complex<T>::Cartesian((temp*aa + ab) / norm, (temp*bb + ba) / norm);
}

ExpressionLayout * Division::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  const Expression * numerator = operand(0)->type() == Type::Parenthesis ? operand(0)->operand(0) : operand(0);
  const Expression * denominator = operand(1)->type() == Type::Parenthesis ? operand(1)->operand(0) : operand(1);
  return new FractionLayout(numerator->createLayout(floatDisplayMode, complexFormat), denominator->createLayout(floatDisplayMode, complexFormat));
}

template<typename T> Matrix * Division::computeOnComplexAndMatrix(const Complex<T> * c, const Matrix * n) {
  Matrix * inverse = n->createInverse<T>();
  if (inverse == nullptr) {
    return nullptr;
  }
  Matrix * result = Multiplication::computeOnComplexAndMatrix<T>(c, inverse);
  delete inverse;
  return result;
}

template<typename T> Matrix * Division::computeOnMatrices(const Matrix * m, const Matrix * n) {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Matrix * inverse = n->createInverse<T>();
  if (inverse == nullptr) {
    return nullptr;
  }
  Matrix * result = Multiplication::computeOnMatrices<T>(m, inverse);
  delete inverse;
  return result;
}

}
