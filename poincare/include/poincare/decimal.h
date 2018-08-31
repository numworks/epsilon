#ifndef POINCARE_DECIMAL_H
#define POINCARE_DECIMAL_H

#include <poincare/integer.h>

namespace Poincare {

/* A decimal as 0.01234 is stored that way:
 *  - bool m_negative = false
 *  - int m_exponent = -2
 *  - int m_numberOfDigitsInMantissa = 1
 *  - native_uint_t m_mantissa[] = { 1234 }
 */

class Decimal;

class DecimalNode : public NumberNode {
  friend class Decimal;
public:
  DecimalNode() :
    m_negative(false),
    m_exponent(0),
    m_numberOfDigitsInMantissa(0) {}

  virtual void setValue(native_uint_t * mantissaDigits, size_t mantissaSize, int exponent, bool negative);

  // Allocation Failure
  static DecimalNode * FailedAllocationStaticNode();
  DecimalNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  NaturalIntegerPointer mantissa() const;
  int exponent() const { return m_exponent; }

  // TreeNode
  size_t size() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Decimal";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    char buffer[256];
    serialize(buffer, sizeof(buffer));
    stream << " value=\"" << buffer << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Decimal; }
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>();
  }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;

  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;
  Expression shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const override;

  // Serialization
  bool needsParenthesesWithParent(const SerializationHelperInterface * e) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
private:
  // Worst case is -1.2345678901234E-1000
  constexpr static int k_maxBufferSize = PrintFloat::k_numberOfStoredSignificantDigits+1+1+1+1+4+1;
  int convertToText(char * buffer, int bufferSize, Preferences::PrintFloatMode mode, int numberOfSignificantDigits) const;
  template<typename T> Evaluation<T> templatedApproximate() const;
  void setNegative(bool negative) { m_negative = negative; }
  bool m_negative;
  int m_exponent;
  size_t m_numberOfDigitsInMantissa;
  native_uint_t m_mantissa[0];
};

class AllocationFailureDecimalNode : public AllocationFailureExpressionNode<DecimalNode> {
public:
  void setValue(native_uint_t * mantissaDigits, size_t mantissaSize, int exponent, bool negative) override {}
};

class Decimal : public Number {
friend class Number;
friend class DecimalNode;
public:
  static int Exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength);
  Decimal(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, int exponent);
  Decimal(const DecimalNode * node) : Number(node) {}
  Decimal(Integer m, int e);
  template <typename T> Decimal(T f);
  constexpr static int k_maxExponentLength = 4;
  constexpr static int k_maxExponent = 1000;
private:
  constexpr static int k_maxMantissaLength = 20;
  DecimalNode * node() const { return static_cast<DecimalNode *>(Number::node()); }
  Decimal(size_t size, const Integer & m, int e);
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit);
  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const;
  Expression shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
