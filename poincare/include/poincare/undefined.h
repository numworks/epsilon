#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

#include <poincare/junior_expression.h>
#include <poincare/number.h>

namespace Poincare {

class UndefinedNode : public NumberNode {
 public:
  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Undefined";
  }
#endif

  // Properties
  Type otype() const override { return Type::Undefined; }
  int polynomialDegree(Context* context, const char* symbolName) const override;
  TrinaryBoolean isNull(Context* context) const override {
    return TrinaryBoolean::Unknown;
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
  template <typename T>
  Evaluation<T> templatedApproximate() const;
  // Simplification
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
};

class Undefined final : public Number {
 public:
  Undefined(const UndefinedNode* n) : Number(n) {}
  static Undefined Builder() {
    return TreeHandle::FixedArityBuilder<Undefined, UndefinedNode>();
  }
  constexpr static const char* Name() { return "undef"; }
  constexpr static int NameSize() { return 6; }
};

class JuniorUndefined final : public JuniorExpression {
 public:
  static JuniorUndefined Builder();
};

}  // namespace Poincare

#endif
