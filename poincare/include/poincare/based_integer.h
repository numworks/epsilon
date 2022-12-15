#ifndef POINCARE_BASED_INTEGER_H
#define POINCARE_BASED_INTEGER_H

#include <poincare/integer.h>
#include <poincare/number.h>
#include <poincare/complex.h>

namespace Poincare {

class BasedIntegerNode final : public NumberNode {
public:
  BasedIntegerNode(const native_uint_t * digits, uint8_t size, OMG::Base base);

  Integer integer() const;
  OMG::Base base() const { return m_base; }

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
  TrinaryBoolean isPositive(Context * context) const override { return TrinaryBoolean::True; }
  TrinaryBoolean isNull(Context * context) const override { return BinaryToTrinaryBool(integer().isZero()); }

  // NumberNode
  void setNegative(bool negative) override { assert(!negative); }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return Complex<float>::Builder(templatedApproximate<float>()); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return Complex<double>::Builder(templatedApproximate<double>()); }
  template<typename T> T templatedApproximate() const;

private:
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return m_base == OMG::Base::Decimal ? LayoutShape::Integer : LayoutShape::Default; }
  OMG::Base m_base;
  uint8_t m_numberOfDigits;
  native_uint_t m_digits[0];
};

class BasedInteger final : public Number {
  friend class BasedIntegerNode;
public:
  /* The constructor build a irreductible fraction */
  BasedInteger(const BasedIntegerNode * node) : Number(node) {}
  static BasedInteger Builder(const char * digits, size_t size, OMG::Base base);
  static BasedInteger Builder(const Integer & m, OMG::Base base = OMG::Base::Decimal);

  // TreeNode
  BasedIntegerNode * node() const { return static_cast<BasedIntegerNode *>(Number::node()); }

  // Properties
  Integer integer() const { return node()->integer(); }
  OMG::Base base() const { return node()->base(); }

private:
  BasedIntegerNode * node() { return static_cast<BasedIntegerNode *>(Number::node()); }

  /* Simplification */
  Expression shallowReduce();
};

}

#endif
