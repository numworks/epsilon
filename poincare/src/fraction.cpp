extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}
#include <cmath>
#include <poincare/fraction.h>
#include <poincare/multiplication.h>
#include "layout/fraction_layout.h"

namespace Poincare {

Expression * Fraction::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Fraction(newOperands, cloneOperands);
}

ExpressionLayout * Fraction::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new FractionLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat), m_operands[1]->createLayout(floatDisplayMode, complexFormat));
}

Expression::Type Fraction::type() const {
  return Type::Fraction;
}

template<typename T>
Complex<T> Fraction::compute(const Complex<T> c, const Complex<T> d) {
  // We want to avoid multiplies in the middle of the calculation that could overflow.
  // aa, ab, ba, bb, min, max = |d.a| <= |d.b| ? (c.a, c.b, -c.a, c.b, d.a, d.b) : (c.b, c.a, c.b, -c.a, d.b, d.a)
  // c    c.a+c.b*i   d.a-d.b*i   1/max    (c.a+c.b*i) * (d.a-d.b*i) / max
  // - == --------- * --------- * ----- == -------------------------------
  // d    d.a+d.b*i   d.a-d.b*i   1/max    (d.a+d.b*i) * (d.a-d.b*i) / max
  //      (c.a*d.a - c.a*d.b*i + c.b*i*d.a - c.b*i*d.b*i) / max
  //   == -----------------------------------------------------
  //      (d.a*d.a - d.a*d.b*i + d.b*i*d.a - d.b*i*d.b*i) / max
  //      (c.a*d.a - c.b*d.b*i^2 + c.b*d.b*i - c.a*d.a*i) / max
  //   == -----------------------------------------------------
  //                  (d.a*d.a - d.b*d.b*i^2) / max
  //      (c.a*d.a/max + c.b*d.b/max) + (c.b*d.b/max - c.a*d.a/max)*i
  //   == -----------------------------------------------------------
  //                         d.a^2/max + d.b^2/max
  //      aa*min/max + ab*max/max   bb*min/max + ba*max/max
  //   == ----------------------- + -----------------------*i
  //       min^2/max + max^2/max     min^2/max + max^2/max
  //       min/max*aa + ab     min/max*bb + ba
  //   == ----------------- + -----------------*i
  //      min/max*min + max   min/max*min + max
  // |min| <= |max| => |min/max| <= 1
  //                => |min/max*x| <= |x|
  //                => |min/max*x+y| <= |x|+|y|
  // So the calculation is guaranteed to not overflow until the last divides as
  // long as none of the input values have the representation's maximum exponent.
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

template<typename T> Evaluation<T> * Fraction::templatedComputeOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * n) const {
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnComplexAndMatrix(c, inverse);
  delete inverse;
  return result;
}

template<typename T> Evaluation<T> * Fraction::templatedComputeOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n) const {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnMatrices(m, inverse);
  delete inverse;
  return result;
}

template Complex<float> Fraction::compute(const Complex<float>, const Complex<float>);
template Complex<double> Fraction::compute(const Complex<double>, const Complex<double>);

}
