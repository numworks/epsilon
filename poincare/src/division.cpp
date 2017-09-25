extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}

#include <poincare/division.h>
#include <poincare/multiplication.h>
#include "layout/fraction_layout.h"

namespace Poincare {

Expression::Type Division::type() const {
  return Type::Division;
}

Expression * Division::clone() const {
  return new Division(m_operands, true);
}

template<typename T>
Complex<T> Division::compute(const Complex<T> c, const Complex<T> d) {
  T norm = d.a()*d.a() + d.b()*d.b();
  /* We handle the case of c and d pure real numbers apart. Even if the complex
   * division is mathematically correct on real numbers, it requires more
   * operations and is thus more likely to propagate errors due to float exact
   * representation. */
  if (d.b() == 0 && c.b() == 0) {
    return Complex<T>::Float(c.a()/d.a());
  }
  return Complex<T>::Cartesian((c.a()*d.a()+c.b()*d.b())/norm, (d.a()*c.b()-c.a()*d.b())/norm);
}

template<typename T> Evaluation<T> * Division::computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n) {
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnComplexAndMatrix(c, inverse);
  delete inverse;
  return result;
}

template<typename T> Evaluation<T> * Division::computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnMatrices(m, inverse);
  delete inverse;
  return result;
}

ExpressionLayout * Division::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new FractionLayout(operand(0)->createLayout(floatDisplayMode, complexFormat), operand(1)->createLayout(floatDisplayMode, complexFormat));
}

}
