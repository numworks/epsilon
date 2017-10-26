#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include <poincare/integer.h>
#include <poincare/static_hierarchy.h>

namespace Poincare {

class Rational : public StaticHierarchy<0> {
public:
  /* The constructor build a irreductible fraction whose sign is on numerator */
  Rational(const Integer numerator, const Integer denominator);
  Rational(const Integer numerator);

  Rational(const Rational & other);
  /*Rational(Rational && other) = default;
  Rational& operator=(const Rational & other) = default;
  Rational& operator=(Rational && other) = default;*/

  // Getter
  const Integer numerator() const;
  const Integer denominator() const;
  void setNegative(bool negative) { m_numerator.setNegative(negative); }
  // Expression subclassing
  Type type() const override;
  Expression * clone() const override;
  int sign() const override;
  Expression * turnIntoPositive(Context & context, AngleUnit angleUnit) override { m_numerator.setNegative(false); return this; }

  // Basic test
  bool isZero() const { return m_numerator.isZero(); }
  bool isOne() const { return m_numerator.isOne() && m_denominator.isOne(); }
  bool isMinusOne() const { return m_numerator.isMinusOne() && m_denominator.isOne(); }
  bool isMinusHalf() const { return m_numerator.isMinusOne() && m_denominator.isTwo(); }

  // Arithmetic
  static Rational Addition(const Rational & i, const Rational & j);
  static Rational Multiplication(const Rational & i, const Rational & j);
  static Rational Power(const Rational & i, const Integer & j);
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename U> Evaluation<U> * templatedEvaluate(Context& context, Expression::AngleUnit angleUnit) const;

  /* Sorting */
  int compareToSameTypeExpression(const Expression * e) const override;

  Integer m_numerator;
  Integer m_denominator;
};

}

#endif
