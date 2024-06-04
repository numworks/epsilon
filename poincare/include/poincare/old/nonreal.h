#ifndef POINCARE_NONREAL_H
#define POINCARE_NONREAL_H

#include "undefined.h"

namespace Poincare {

class NonrealNode final : public UndefinedNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(NonrealNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "nonreal"; }
#endif

  // Properties
  Type otype() const override { return Type::Nonreal; }

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
   * Unlike Numbers that derivate to 0, Nonreal derivates to Nonreal. */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const override;

 private:
  template <typename T>
  Evaluation<T> templatedApproximate() const {
    OExpression::SetEncounteredComplex(true);
    return Complex<T>::Undefined();
  }
};

class Nonreal final : public Number {
 public:
  static Nonreal Builder() {
    return PoolHandle::FixedArityBuilder<Nonreal, NonrealNode>();
  }
  Nonreal() = delete;
  constexpr static const char* Name() { return "nonreal"; }
  static int NameSize() { return 8; }
};

}  // namespace Poincare

#endif
