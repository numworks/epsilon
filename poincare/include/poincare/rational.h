#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include <poincare/integer.h>
#include <poincare/static_hierarchy.h>

namespace Poincare {

class Rational : public StaticHierarchy<0> {
  friend class Power;
  friend class Multiplication;
public:
  /* The constructor build a irreductible fraction whose sign is on numerator */
  Rational(const Integer numerator, const Integer denominator);
  Rational(const Integer numerator);
  Rational(Integer::native_int_t i) : Rational(Integer(i)) {}

  Rational(const Rational & other);
  /*Rational(Rational && other) = default;
  Rational& operator=(const Rational & other) = default;
  Rational& operator=(Rational && other) = default;*/

  // Getter
  const Integer numerator() const;
  const Integer denominator() const;
  // Expression subclassing
  Type type() const override;
  Expression * clone() const override;
  Sign sign() const override;
  Expression * cloneDenominator(Context & context, AngleUnit angleUnit) const override;

  // Basic test
  bool isZero() const { return m_numerator.isZero(); }
  bool isOne() const { return m_numerator.isOne() && m_denominator.isOne(); }
  bool isMinusOne() const { return m_numerator.isMinusOne() && m_denominator.isOne(); }
  bool isHalf() const { return m_numerator.isOne() && m_denominator.isTwo(); }
  bool isMinusHalf() const { return m_numerator.isMinusOne() && m_denominator.isTwo(); }

  // Arithmetic
  static Rational Addition(const Rational & i, const Rational & j);
  static Rational Multiplication(const Rational & i, const Rational & j);
  static Rational Power(const Rational & i, const Integer & j);
  static int NaturalOrder(const Rational & i, const Rational & j);
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
  template<typename U> Evaluation<U> * templatedEvaluate(Context& context, Expression::AngleUnit angleUnit) const;
  Expression * shallowBeautify(Context & context, AngleUnit angleUnit) override;
  Expression * setSign(Sign s);
  Expression * setSign(Sign s, Context & context, AngleUnit angleUnit) override {
    return setSign(s);
  }

  /* Sorting */
  int simplificationOrderSameType(const Expression * e) const override;

  Integer m_numerator;
  Integer m_denominator;
};

}

#endif
