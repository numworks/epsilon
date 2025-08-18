#ifndef POINCARE_HYPERBOLIC_ARC_COSINE_H
#define POINCARE_HYPERBOLIC_ARC_COSINE_H

#include "approximation_helper.h"
#include "hyperbolic_trigonometric_function.h"

namespace Poincare {

class HyperbolicArcCosineNode final
    : public HyperbolicTrigonometricFunctionNode {
 public:
  constexpr static AliasesList k_functionName = "arcosh";

  // PoolObject
  size_t size() const override { return sizeof(HyperbolicArcCosineNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "HyperbolicArcCosine";
  }
#endif

  // Properties
  Type otype() const override { return Type::HyperbolicArcCosine; }

 private:
  // Simplification
  bool isNotableValue(OExpression e, Context* context) const override {
    return e.isRationalOne();
  }
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
};

class HyperbolicArcCosine final
    : public ExpressionOneChild<HyperbolicArcCosine, HyperbolicArcCosineNode,
                                HyperbolicTrigonometricFunction> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
