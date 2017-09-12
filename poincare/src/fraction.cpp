extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}

#include <poincare/fraction.h>
#include <poincare/multiplication.h>
#include "layout/fraction_layout.h"

namespace Poincare {

Expression * Fraction::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands >= 2);
  assert(newOperands != nullptr);
  return new Fraction(newOperands, numberOfOperands, cloneOperands);
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
  T norm = d.a()*d.a() + d.b()*d.b();
  /* We handle the case of c and d pure real numbers apart. Even if the complex
   * fraction is mathematically correct on real numbers, it requires more
   * operations and is thus more likely to propagate errors due to float exact
   * representation. */
  if (d.b() == 0 && c.b() == 0) {
    return Complex<T>::Float(c.a()/d.a());
  }
  return Complex<T>::Cartesian((c.a()*d.a()+c.b()*d.b())/norm, (d.a()*c.b()-c.a()*d.b())/norm);
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

}
