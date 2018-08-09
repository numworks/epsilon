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

class DecimalNode : public NumberNode {
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
  size_t size() const override { return sizeof(DecimalNode); }
#if TREE_LOG
  const char * description() const override { return "Decimal";  }
#endif

  // Properties
  Type type() const override { return Type::Decimal; }
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }

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
  static int Exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength, bool exponentNegative);
  Decimal(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, bool negative, int exponent);
  Decimal(const DecimalNode * node) : Number(node) {}
  constexpr static int k_maxExponentLength = 4;
private:
  DecimalNode * node() const override { return static_cast<DecimalNode *>(Number::node()); }
  template <typename T> Decimal(T f);
  Decimal(Integer m, int e);
  Decimal(size_t size) : Number(nullptr) {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<DecimalNode>(size);
    m_identifier = node->identifier();
  }
  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const;
  Expression shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
