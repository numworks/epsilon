#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

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
  Type type() const override { return Type::Undefined; }
  int polynomialDegree(Context* context, const char* symbolName) const override;

  // NumberNode
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
                Expression symbolValue) override;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
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

}  // namespace Poincare

#endif
