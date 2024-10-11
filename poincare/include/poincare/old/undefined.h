#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

#include "junior_expression.h"
#include "number.h"

namespace Poincare {

class UndefinedNode : public NumberNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(UndefinedNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Undefined";
  }
#endif

  // Properties
  Type otype() const override { return Type::Undefined; }
  OMG::Troolean isNull(Context* context) const override {
    return OMG::Troolean::Unknown;
  }

  // NumberNode
  bool isZero() const override { return false; }
  bool isOne() const override { return false; }
  bool isMinusOne() const override { return false; }
  bool isInteger() const override { return false; }
  Integer integerValue() const override {
    assert(false);
    return Integer();
  }
  void setNegative(bool negative) override {}

  /* Derivation
   * Unlike Numbers that derivate to 0, Undefined derivates to Undefined. */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const override;

 protected:
  // Simplification
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
};

class OUndefined final : public Number {
 public:
  OUndefined(const UndefinedNode* n) : Number(n) {}
  static OUndefined Builder() {
    return PoolHandle::FixedArityBuilder<OUndefined, UndefinedNode>();
  }
};

}  // namespace Poincare

#endif
