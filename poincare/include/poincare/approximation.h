#ifndef POINCARE_APPROXIMATION_H
#define POINCARE_APPROXIMATION_H

#include <poincare/expression.h>

namespace Poincare {

/* Approximation reprensents an approximated complex. This class is
 * theoritecally unecessary as it can be reprensented by an Addition of Decimals
 * and I Symbol for example. However, performances are dramatically affected if
 * we keep turning float/double into Decimal back and forth (for instance, when
 * plotting a graph, we need to set a float/double value for a symbol and
 * approximate an expression containing the symbol for each dot displayed). We
 * thus use the Approximation class that hold an std::complex<T> and can only
 * be approximated. Indeed, the usual methods of expression are not implemented
 * to avoid code duplication with Decimal. */

template<typename T>
class Approximation : public Expression {
public:
  Approximation(T a, T b = 0.0) :
    Expression(),
    m_complex(a, b)
  {}
  Type type() const override {
    return Expression::Type::Approximation;
  }
  Expression * clone() const override {
    return new Approximation(m_complex.real(), m_complex.imag());
  }
  const Expression * const * operands() const override {
    assert(false);
    return nullptr;
  }
  int numberOfOperands() const override {
    assert(false);
    return 0;
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    assert(false);
    return 0;
  }
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
   assert(false);
   return nullptr;
  }
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename U> Complex<U> * templatedApproximate(Context& context, AngleUnit angleUnit) const {
    return new Complex<U>((U)m_complex.real(), (U)m_complex.imag());
  }
  std::complex<T> m_complex;
};

}

#endif
