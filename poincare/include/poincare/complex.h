#ifndef POINCARE_COMPLEX_H
#define POINCARE_COMPLEX_H

#include <poincare/preferences.h>
#include <poincare/static_hierarchy.h>
#include <assert.h>

namespace Poincare {

template<typename T>
class Complex final : public StaticHierarchy<0> {
public:
  Complex() : m_a(0), m_b(0) {}
  static Complex<T> Float(T x);
  static Complex<T> * NewFloat(T x);
  static Complex<T> Cartesian(T a, T b);/* {
    return Complex(a,b);
  }*/
  static Complex<T> Polar(T r, T theta);
  static Complex<T> FNAN();
  static Complex<T> * NewFNAN();
  static Complex<T> FINF();
  static Complex<T> * NewFINF();
  Complex(const char * integralPart, int integralPartLength, bool integralNegative,
        const char * fractionalPart, int fractionalPartLength,
        const char * exponent, int exponentLength, bool exponentNegative);
  Complex(const Complex & other);
  Complex& operator=(const Complex& other);

  T a() const { return m_a; }
  T b() const { return m_b; }
  T r() const;
  T th() const;
  Complex<T> conjugate() const { return Cartesian(m_a, -m_b); }
  T toScalar() const;

  /* Expression */
  Expression::Type type() const override;
  Complex<T> * clone() const override {
    return new Complex<T>(*this);
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

  /* Properties */
  bool needParenthesisWithParent(const Expression * e) const override;

  /* Simplification: complex does not implement simplificationOrderSameType
   * because Complex expressions are always transformed into an addition of
   * Decimal and I symbol before compared with another Expression. */

private:
  Complex(T a, T b);/* : m_a(a), m_b(b)
  {
  }*/
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat) const override;
  /* Simplification */
  static Expression * CreateDecimal(T f);
  Expression * shallowReduce(Context & context, AngleUnit angleUnit) override;
  /* Evaluation */
  Expression * privateApproximate(Expression::SinglePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(Expression::DoublePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename U> Complex<U> * templatedApproximate(Context& context, Expression::AngleUnit angleUnit) const {
    return new Complex<U>(Complex<U>::Cartesian((U)m_a, (U)m_b));
  }
  /* convertComplexToText and convertFloatToTextPrivate return the string length
   * of the buffer (does not count the 0 last char)*/
  int convertComplexToText(char * buffer, int bufferSize, int numberOfSignificantDigits, PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat, char multiplicationSign) const;
  ExpressionLayout * createPolarLayout(PrintFloat::Mode floatDisplayMode) const;
  ExpressionLayout * createCartesianLayout(PrintFloat::Mode floatDisplayMode) const;
  T m_a;
  T m_b;
};

}

#endif
