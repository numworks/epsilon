#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include <poincare/integer.h>
#include <poincare/number.h>
#include <poincare/complex.h>

namespace Poincare {

class RationalNode final : public NumberNode {
public:
  RationalNode() :
    m_negative(false),
    m_numberOfDigitsNumerator(0),
    m_numberOfDigitsDenominator(0) {}
  virtual void setDigits(const native_uint_t * i, uint8_t numeratorSize, const native_uint_t * j, uint8_t denominatorSize, bool negative);

  Integer signedNumerator() const;
  Integer unsignedNumerator() const;
  Integer denominator() const;
  bool isNegative() const { return m_negative; }
  void setNegative(bool negative) { m_negative = negative; }

  // TreeNode
  void initToMatchSize(size_t goalSize) override;
  size_t size() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Rational";
  }
  virtual void logAttributes(std::ostream & stream) const override;
#endif

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Expression subclassing
  Type type() const override { return Type::Rational; }
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>(templatedApproximate<float>()); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>(templatedApproximate<double>()); }
  template<typename T> T templatedApproximate() const;

  // Basic test
  bool isZero() const { return unsignedNumerator().isZero(); }
  bool isOne() const { return signedNumerator().isOne() && denominator().isOne(); }
  bool isMinusOne() const { return signedNumerator().isMinusOne() && denominator().isOne(); }
  bool isHalf() const { return signedNumerator().isOne() && denominator().isTwo(); }
  bool isMinusHalf() const { return signedNumerator().isMinusOne() && denominator().isTwo(); }
  bool isTen() const { return signedNumerator().isTen() && denominator().isOne(); }

  static int NaturalOrder(const RationalNode * i, const RationalNode * j);
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override;
  bool m_negative;
  uint8_t m_numberOfDigitsNumerator;
  uint8_t m_numberOfDigitsDenominator;
  native_uint_t m_digits[0];
};

class Rational final : public Number {
  friend class RationalNode;
  friend class PowerNode;
  friend class Power;
public:
  /* The constructor build a irreductible fraction */
  Rational(const RationalNode * node) : Number(node) {}
  Rational(Integer & num, Integer & den);
  Rational(const Integer & numerator);
  Rational(native_int_t i);
  Rational(native_int_t i, native_int_t j);
  Rational(const char * iString, const char * jString);

  // TreeNode
  RationalNode * node() const { return static_cast<RationalNode *>(Number::node()); }

  // Properties
  Integer signedIntegerNumerator() const { return node()->signedNumerator(); }
  Integer unsignedIntegerNumerator() const { return node()->unsignedNumerator(); }
  Integer integerDenominator() const { return node()->denominator(); }

  // BasicTest
  bool isNegative() const { return node()->isNegative(); }
  bool isZero() const { return node()->isZero(); }
  bool isOne() const { return node()->isOne(); }
  bool isMinusOne() const { return node()->isMinusOne(); }
  bool isHalf() const { return node()->isHalf(); }
  bool isMinusHalf() const { return node()->isMinusHalf(); }
  bool isTen() const { return node()->isTen(); }
  bool numeratorOrDenominatorIsInfinity() const;

  // Arithmetic
  /* Warning: when using this function, always assert that the result does not
   * involve infinity numerator or denominator or handle these cases. */
  static Rational Addition(const Rational & i, const Rational & j);
  static Rational Multiplication(const Rational & i, const Rational & j);
  // IntegerPower of (p1/q1)^(p2) --> (p1^p2)/(q1^p2)
  static Rational IntegerPower(const Rational & i, const Integer & j);
  static int NaturalOrder(const Rational & i, const Rational & j) { return RationalNode::NaturalOrder(i.node(), j.node()); }

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);

private:
  Rational(const native_uint_t * i, uint8_t numeratorSize, const native_uint_t * j, uint8_t denominatorSize, bool negative);
  RationalNode * node() { return static_cast<RationalNode *>(Number::node()); }

  /* Simplification */
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const;
  Expression setSign(ExpressionNode::Sign s);
};

}

#endif
