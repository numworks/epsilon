#ifndef POINCARE_BASED_INTEGER_H
#define POINCARE_BASED_INTEGER_H

#include <poincare/integer.h>
#include <poincare/number.h>
#include <poincare/complex.h>

namespace Poincare {

class BasedIntegerNode final : public NumberNode {
public:
  BasedIntegerNode(const native_uint_t * digits, uint8_t size, Integer::Base base);

  Integer integer() const;

  // TreeNode
  size_t size() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "BasedInteger";
  }
  void logAttributes(std::ostream & stream) const override;
#endif

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Expression subclassing
  Type type() const override { return Type::BasedInteger; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  NullStatus nullStatus(Context * context) const override { return integer().isZero() ? NullStatus::Null : NullStatus::NonNull; }

  // NumberNode
  void setNegative(bool negative) override { assert(!negative); }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return Complex<float>::Builder(templatedApproximate<float>()); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return Complex<double>::Builder(templatedApproximate<double>()); }
  template<typename T> T templatedApproximate() const;

private:
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const override;
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return m_base == Integer::Base::Decimal ? LayoutShape::Integer : LayoutShape::BinaryHexadecimal; }
  Integer::Base m_base;
  uint8_t m_numberOfDigits;
  native_uint_t m_digits[0];
};

class BasedInteger final : public Number {
  friend class BasedIntegerNode;
public:
  /* The constructor build a irreductible fraction */
  BasedInteger(const BasedIntegerNode * node) : Number(node) {}
  static BasedInteger Builder(const char * digits, size_t size, Integer::Base base);
  static BasedInteger Builder(const Integer & m, Integer::Base base = Integer::Base::Decimal);

  // TreeNode
  BasedIntegerNode * node() const { return static_cast<BasedIntegerNode *>(Number::node()); }

  // Properties
  Integer integer() const { return node()->integer(); }

private:
  BasedIntegerNode * node() { return static_cast<BasedIntegerNode *>(Number::node()); }

  /* Simplification */
  Expression shallowReduce();
};

}

#endif
