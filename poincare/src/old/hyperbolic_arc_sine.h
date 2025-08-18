#ifndef POINCARE_HYPERBOLIC_ARC_SINE_H
#define POINCARE_HYPERBOLIC_ARC_SINE_H

#include "approximation_helper.h"
#include "hyperbolic_trigonometric_function.h"

namespace Poincare {

class HyperbolicArcSineNode final : public HyperbolicTrigonometricFunctionNode {
 public:
  constexpr static AliasesList k_functionName = "arsinh";

  // PoolObject
  size_t size() const override { return sizeof(HyperbolicArcSineNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "HyperbolicArcSine";
  }
#endif

  // Properties
  Type otype() const override { return Type::HyperbolicArcSine; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
};

class HyperbolicArcSine final
    : public ExpressionOneChild<HyperbolicArcSine, HyperbolicArcSineNode,
                                HyperbolicTrigonometricFunction> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
