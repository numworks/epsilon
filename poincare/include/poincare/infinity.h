#ifndef POINCARE_INFINITY_H
#define POINCARE_INFINITY_H

#include <poincare/helpers.h>
#include <poincare/number.h>

namespace Poincare {

class InfinityNode final : public NumberNode {
 public:
  InfinityNode(bool negative) : NumberNode(), m_negative(negative) {}

  // TreeNode
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
  Type type() const override { return Type::Infinity; }
  TrinaryBoolean isPositive(Context* context) const override {
    return BinaryToTrinaryBool(!m_negative);
  }
  TrinaryBoolean isNull(Context* context) const override {
    return TrinaryBoolean::False;
  }

  // NumberNode
  void setNegative(bool negative) override { m_negative = negative; }

  // Approximation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>();
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode =
                    Preferences::PrintFloatMode::Decimal,
                int numberOfSignificantDigits = 0) const override;

  /* Derivation
   * Unlike Numbers that derivate to 0, Infinity derivates to Undefined. */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override;

 private:
  // Simplification
  LayoutShape leftLayoutShape() const override {
    assert(!m_negative);
    return LayoutShape::MoreLetters;
  }
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::MoreLetters;
  }
  template <typename T>
  Evaluation<T> templatedApproximate() const;
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
  Expression setSign(bool positive);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue);

 private:
  InfinityNode* node() const {
    return static_cast<InfinityNode*>(Number::node());
  }
};

}  // namespace Poincare

#endif
