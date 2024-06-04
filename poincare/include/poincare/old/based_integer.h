#ifndef POINCARE_BASED_INTEGER_H
#define POINCARE_BASED_INTEGER_H

#include "complex.h"
#include "integer.h"
#include "number.h"

namespace Poincare {

class BasedIntegerNode final : public NumberNode {
 public:
  BasedIntegerNode(const native_uint_t* digits, uint8_t size, OMG::Base base);

  Integer integer() const;
  OMG::Base base() const { return m_base; }

  // PoolObject
  size_t size() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "BasedInteger";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  // SerializationHelperInterface
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // OExpression subclassing
  Type otype() const override { return Type::BasedInteger; }
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }

  // NumberNode
  void setNegative(bool negative) override { assert(!negative); }
  bool isZero() const override { return integer().isZero(); }
  bool isOne() const override { return integer().isOne(); }
  bool isMinusOne() const override { return integer().isMinusOne(); }
  bool isInteger() const override { return true; }
  Integer integerValue() const override { return integer(); }

 private:
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return m_base == OMG::Base::Decimal ? LayoutShape::Integer
                                        : LayoutShape::Default;
  }
  OMG::Base m_base;
  uint8_t m_numberOfDigits;
  native_uint_t m_digits[0];
};

class BasedInteger final : public Number {
  friend class BasedIntegerNode;

 public:
  /* The constructor build a irreductible fraction */
  BasedInteger(const BasedIntegerNode* node) : Number(node) {}
  static BasedInteger Builder(const char* digits, size_t size, OMG::Base base);
  static BasedInteger Builder(const Integer& m,
                              OMG::Base base = OMG::Base::Decimal);

  // PoolObject
  BasedIntegerNode* node() const {
    return static_cast<BasedIntegerNode*>(Number::node());
  }

  // Properties
  Integer integer() const { return node()->integer(); }
  OMG::Base base() const { return node()->base(); }

 private:
  BasedIntegerNode* node() {
    return static_cast<BasedIntegerNode*>(Number::node());
  }

  /* Simplification */
  OExpression shallowReduce();
};

}  // namespace Poincare

#endif
