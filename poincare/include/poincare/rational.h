#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include <poincare/integer.h>

namespace Poincare {

class RationalNode : public NumberNode {
public:
  void setDigits(native_uint_t * i, size_t numeratorSize, native_uint_t * j, size_t denominatorSize, bool negative);

  NaturalIntegerPointer numerator() const;
  NaturalIntegerPointer denominator() const;

  // TreeNode
  size_t size() const override;
#if TREE_LOG
  const char * description() const override { return "Rational";  }
#endif

  // Serialization Node
  bool needsParenthesisWithParent(SerializableNode * e) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Expression subclassing
  Type type() const override { return Type::Rational; }
  Sign sign() const override;
  void setSign(Sign s);

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return ComplexReference<float>(templatedApproximate<float>()); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return ComplexReference<double>(templatedApproximate<double>()); }
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
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override {
    setSign(s);
    return Expression(this);
  }
  Expression cloneDenominator(Context & context, Preferences::AngleUnit angleUnit) const override;
  bool m_negative;
  size_t m_numberOfDigitsNumerator;
  size_t m_numberOfDigitsDenominator;
  native_uint_t m_digits[0];
};

class RationalReference : public NumberReference {
public:
  /* The constructor build a irreductible fraction */
  RationalReference(IntegerReference numerator, IntegerReference denominator);
  RationalReference(const IntegerReference numerator);
  RationalReference(const NaturalIntegerAbstract * numerator, bool negative);
  RationalReference(native_int_t i);
  RationalReference(native_int_t i, native_int_t j);

  // TreeNode
  RationalNode * typedNode() const { assert(node()->type() == ExpressionNode::Type::Rational); return static_cast<RationalNode *>(node()); }

  // Properties
  bool isOne() const;
  bool numeratorOrDenominatorIsInfinity() const;

  // Arithmetic
  /* Warning: when using this function, always assert that the result does not
   * involve infinity numerator or denominator or handle these cases. */
  static RationalReference Addition(const RationalReference i, const RationalReference j);
  static RationalReference Multiplication(const RationalReference i, const RationalReference j);
  // IntegerPower of (p1/q1)^(p2/q2) --> (p1^p2)/(q1^p2)
  static RationalReference IntegerPower(const RationalReference i, const RationalReference j);
  static int NaturalOrder(const RationalReference i, const RationalReference j);

private:
  RationalReference(size_t size) : NumberReference(TreePool::sharedPool()->createTreeNode<RationalNode>(size)) {}
};

}

#endif
