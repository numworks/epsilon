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
  void setValue(native_uint_t * mantissaDigits, size_t mantissaSize, int exponent, bool negative);

  NaturalIntegerPointer mantissa() const;

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
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  Expression shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) override;

  // Layout
  bool needsParenthesesWithParent(SerializableNode * parentNode) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
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

class DecimalReference : public NumberReference {
friend class Number;
public:
  static int exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength, bool exponentNegative);
  DecimalReference(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, bool negative, int exponent);
  constexpr static int k_maxExponentLength = 4;
private:
  DecimalReference(TreeNode * n) : NumberReference(n) {}
  template <typename T> DecimalReference(T f);
  DecimalReference(IntegerReference m, int e);
  DecimalReference(size_t size) : NumberReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<DecimalNode>(size);
    m_identifier = node->identifier();
  }
};

}

#endif
