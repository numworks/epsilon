#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include <poincare/integer.h>

namespace Poincare {

class RationalNode : public NumberNode {
public:
  RationalNode() :
    m_negative(false),
    m_numberOfDigitsNumerator(0),
    m_numberOfDigitsDenominator(0) {}
  virtual void setDigits(native_uint_t * i, size_t numeratorSize, native_uint_t * j, size_t denominatorSize, bool negative);

  // Allocation Failure
  static RationalNode * FailedAllocationStaticNode();
  RationalNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  NaturalIntegerPointer numerator() const;
  NaturalIntegerPointer denominator() const;
  void setNegative(bool negative) { m_negative = negative; }

  // TreeNode
  size_t size() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Rational";
  }
  virtual void logAttributes(std::ostream & stream) const override;
#endif

  // SerializationHelperInterface
  bool needsParenthesesWithParent(const SerializationHelperInterface * e) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Expression subclassing
  Type type() const override { return Type::Rational; }
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>(templatedApproximate<float>()); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>(templatedApproximate<double>()); }
  template<typename T> T templatedApproximate() const;

  // Basic test
  bool isZero() const { return numerator().isZero(); }
  bool isOne() const { return numerator().isOne() && denominator().isOne() && !m_negative; }
  bool isMinusOne() const { return numerator().isOne() && denominator().isOne() && m_negative; }
  bool isHalf() const { return numerator().isOne() && denominator().isTwo() && !m_negative; }
  bool isMinusHalf() const { return numerator().isOne() && denominator().isTwo() && m_negative; }
  bool isTen() const { return numerator().isTen() && denominator().isOne() && !m_negative; }

  static int NaturalOrder(const RationalNode * i, const RationalNode * j);
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) const override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const override;
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const override;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override;
  bool m_negative;
  size_t m_numberOfDigitsNumerator;
  size_t m_numberOfDigitsDenominator;
  native_uint_t m_digits[0];
};

class AllocationFailureRationalNode : public AllocationFailureExpressionNode<RationalNode> {
public:
  void setDigits(native_uint_t * i, size_t numeratorSize, native_uint_t * j, size_t denominatorSize, bool negative) override {};
};

class Rational : public Number {
  friend class RationalNode;
  friend class PowerNode;
  friend class Power;
public:
  /* The constructor build a irreductible fraction */
  Rational(const RationalNode * node) : Number(node) {}
  Rational(Integer numerator, Integer denominator);
  Rational(const Integer numerator);
  Rational(native_int_t i);
  Rational(native_int_t i, native_int_t j) : Rational(Integer(i), Integer(j)) {}

  // TreeNode
  RationalNode * node() const override { return static_cast<RationalNode *>(Number::node()); }

  // Properties
  Integer signedIntegerNumerator() const;
  Integer unsignedIntegerNumerator() const;
  Integer integerDenominator() const;

  // BasicTest
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
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) const;

private:
  Rational(size_t size, native_uint_t * i, size_t numeratorSize, native_uint_t * j, size_t denominatorSize, bool negative);
  Rational(const NaturalIntegerAbstract * numerator, bool negative);
  RationalNode * node() { return static_cast<RationalNode *>(Number::node()); }

  /* Simplification */
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const;
  Expression setSign(ExpressionNode::Sign s) const;
};

}

#endif
