#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include <poincare/integer.h>

namespace Poincare {

class RationalNode : public NumberNode {
public:
  RationalNode() :
    m_numberOfDigitsNumerator(0),
    m_numberOfDigitsDenominator(0) {}
  virtual void setDigits(native_uint_t * i, size_t numeratorSize, native_uint_t * j, size_t denominatorSize, bool negative);

  // Allocation Failure
  static RationalNode * FailedAllocationStaticNode();

  NaturalIntegerPointer numerator() const;
  NaturalIntegerPointer denominator() const;

  // TreeNode
  size_t size() const override;
#if TREE_LOG
  const char * description() const override { return "Rational";  }
#endif

  // SerializationHelperInterface
  bool needsParenthesesWithParent(const SerializationHelperInterface * e) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Expression subclassing
  Type type() const override { return Type::Rational; }
  Sign sign() const override;

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

  static int NaturalOrder(const RationalNode i, const RationalNode j);
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const override;
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;
  Expression cloneDenominator(Context & context, Preferences::AngleUnit angleUnit) const override;
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
public:
  /* The constructor build a irreductible fraction */
  Rational(Integer numerator, Integer denominator);
  Rational(const Integer numerator);
  Rational(const NaturalIntegerAbstract * numerator, bool negative);
  Rational(native_int_t i);
  Rational(native_int_t i, native_int_t j);

  // TreeNode
  RationalNode * node() const override { return static_cast<RationalNode *>(Number::node()); }

  // Properties
  bool isOne() const;
  bool numeratorOrDenominatorIsInfinity() const;

  // Arithmetic
  /* Warning: when using this function, always assert that the result does not
   * involve infinity numerator or denominator or handle these cases. */
  static Rational Addition(const Rational i, const Rational j);
  static Rational Multiplication(const Rational i, const Rational j);
  // IntegerPower of (p1/q1)^(p2/q2) --> (p1^p2)/(q1^p2)
  static Rational IntegerPower(const Rational i, const Rational j);
  static int NaturalOrder(const Rational i, const Rational j);

private:
  Rational(size_t size) : Number(TreePool::sharedPool()->createTreeNode<RationalNode>(size)) {}

  /* Simplification */
  Expression setSign(ExpressionNode::Sign s);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const;
  Expression cloneDenominator(Context & context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
