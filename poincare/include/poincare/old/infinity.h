#ifndef POINCARE_INFINITY_H
#define POINCARE_INFINITY_H

#include "helpers.h"
#include "number.h"

namespace Poincare {

class InfinityNode final : public NumberNode {
 public:
  InfinityNode(bool negative) : NumberNode(), m_negative(negative) {}

  // PoolObject
  size_t size() const override { return sizeof(InfinityNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Infinity";
  }
  void logAttributes(std::ostream& stream) const override {
    stream << " negative=\"" << m_negative << "\"";
  }
#endif

  // Properties
  Type otype() const override { return Type::Infinity; }
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::BoolToTroolean(!m_negative);
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
  void setNegative(bool negative) override { m_negative = negative; }

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const override;

  /* Derivation
   * Unlike Numbers that derivate to 0, Infinity derivates to Undefined. */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

 private:
  // Simplification
  LayoutShape leftLayoutShape() const override {
    assert(!m_negative);
    return LayoutShape::MoreLetters;
  }
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::MoreLetters;
  }
  bool m_negative;
};

class Infinity final : public Number {
 public:
  constexpr static AliasesList k_infinityAliases =
      AliasesLists::k_infinityAliases;
  /* A constexpr of this is needed so that the Name() function can return a
   * const char * when negative = true */
  constexpr static const char* k_minusInfinityMainName = "-∞";
  // ∞ is 3 chars long.
  static_assert(Helpers::StringsAreEqual(k_minusInfinityMainName + 1,
                                         k_infinityAliases.mainAlias()),
                "minus infinity does not have same name as infinity.");

  constexpr static const char* Name(bool negative = false) {
    return negative ? k_minusInfinityMainName : k_infinityAliases.mainAlias();
  }
  constexpr static int NameSize(bool negative = false) {
    return Helpers::StringLength(Name(negative)) + 1;
  }
  static Infinity Builder(bool negative);

  Infinity(InfinityNode* n) : Number(n) {}
  OExpression setSign(bool positive);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);

 private:
  InfinityNode* node() const {
    return static_cast<InfinityNode*>(Number::node());
  }
};

}  // namespace Poincare

#endif
