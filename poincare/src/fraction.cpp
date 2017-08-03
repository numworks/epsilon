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

Complex Fraction::compute(const Complex c, const Complex d) {
  float norm = d.a()*d.a() + d.b()*d.b();
  /* We handle the case of c and d pure real numbers apart. Even if the complex
   * fraction is mathematically correct on real numbers, it requires more
   * operations and is thus more likely to propagate errors due to float exact
   * representation. */
  if (d.b() == 0.0f && c.b() == 0.0f) {
    return Complex::Float(c.a()/d.a());
  }
  return Complex::Cartesian((c.a()*d.a()+c.b()*d.b())/norm, (d.a()*c.b()-c.a()*d.b())/norm);
}

Evaluation * Fraction::computeOnComplexAndComplexMatrix(const Complex * c, Evaluation * m) const {
  Evaluation * inverse = m->createInverse();
  Evaluation * result = Multiplication::computeOnComplexAndMatrix(c, inverse);
  delete inverse;
  return result;
}

Evaluation * Fraction::computeOnNumericalMatrices(Evaluation * m, Evaluation * n) const {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Evaluation * inverse = n->createInverse();
  Evaluation * result = Multiplication::computeOnMatrices(m, inverse);
  delete inverse;
  return result;
}

}
