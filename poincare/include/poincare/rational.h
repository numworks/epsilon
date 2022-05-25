#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include <poincare/integer.h>
#include <poincare/number.h>
#include <poincare/complex.h>

namespace Poincare {

class RationalNode final : public NumberNode {
public:
  RationalNode(const native_uint_t * i, uint8_t numeratorSize, const native_uint_t * j, uint8_t denominatorSize, bool negative);

  Integer signedNumerator() const;
  Integer unsignedNumerator() const;
  Integer denominator() const;
  bool isNegative() const { return m_negative; }
  void setNegative(bool negative) override { m_negative = negative; }
  bool isInteger() const { return denominator().isOne(); }
  NullStatus nullStatus(Context * context) const override { return isZero() ? NullStatus::Null : NullStatus::NonNull; }

  // TreeNode
  size_t size() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Rational";
  }
  virtual void logAttributes(std::ostream & stream) const override;
#endif

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Expression subclassing
  Type type() const override { return Type::Rational; }
  Sign sign(Context * context) const override { return m_negative ? Sign::Negative : Sign::Positive; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return Complex<float>::Builder(templatedApproximate<float>()); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return Complex<double>::Builder(templatedApproximate<double>()); }
  template<typename T> T templatedApproximate() const;

  // Basic test
  bool isZero() const { return unsignedNumerator().isZero(); }
  bool isOne() const { return signedNumerator().isOne() && isInteger(); }
  bool isTwo() const { return signedNumerator().isTwo() && isInteger(); }
  bool isThree() const { return signedNumerator().isThree() && isInteger(); }
  bool isMinusOne() const { return signedNumerator().isMinusOne() && isInteger(); }
  bool isMinusTwo() const { return signedNumerator().isMinusTwo() && isInteger(); }
  bool isHalf() const { return signedNumerator().isOne() && denominator().isTwo(); }
  bool isMinusHalf() const { return signedNumerator().isMinusOne() && denominator().isTwo(); }
  bool isTen() const { return signedNumerator().isTen() && isInteger(); }
  bool isGreaterThanOne() const { return denominator().isLowerThan(signedNumerator()); }

  static int NaturalOrder(const RationalNode * i, const RationalNode * j);
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const override;
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext * reductionContext) override;
  LayoutShape leftLayoutShape() const override { assert(!m_negative); return isInteger() ? LayoutShape::Integer : LayoutShape::Fraction; };
  Expression denominator(ReductionContext reductionContext) const override;
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
  static Rational Builder(Integer & num, Integer & den);
  static Rational Builder(const Integer & numerator);
  static Rational Builder(native_int_t i);
  static Rational Builder(native_int_t i, native_int_t j);
  static Rational Builder(const char * iString, const char * jString);

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
  bool isTwo() const { return node()->isTwo(); }
  bool isThree() const { return node()->isThree(); }
  bool isMinusOne() const { return node()->isMinusOne(); }
  bool isMinusTwo() const { return node()->isMinusTwo(); }
  bool isHalf() const { return node()->isHalf(); }
  bool isMinusHalf() const { return node()->isMinusHalf(); }
  bool isTen() const { return node()->isTen(); }
  bool isGreaterThanOne() const { return node()->isGreaterThanOne(); }
  bool numeratorOrDenominatorIsInfinity() const;
  bool isInteger() const { return node()->isInteger(); }

  // Arithmetic
  /* Warning: when using this function, always assert that the result does not
   * involve infinity numerator or denominator or handle these cases. */
  static Rational Addition(const Rational & i, const Rational & j);
  static Rational Multiplication(const Rational & i, const Rational & j);
  // IntegerPower of (p1/q1)^(p2) --> (p1^p2)/(q1^p2)
  static Rational IntegerPower(const Rational & i, const Integer & j);
  static int NaturalOrder(const Rational & i, const Rational & j) { return RationalNode::NaturalOrder(i.node(), j.node()); }

  // Simplification
  Expression shallowReduce();

private:
  static Rational Builder(const native_uint_t * i, uint8_t numeratorSize, const native_uint_t * j, uint8_t denominatorSize, bool negative);

  /* Simplification */
  Expression shallowBeautify();
  Expression denominator() const;
};

}

#endif
