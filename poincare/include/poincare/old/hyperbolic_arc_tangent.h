#ifndef POINCARE_HYPERBOLIC_ARC_TANGENT_H
#define POINCARE_HYPERBOLIC_ARC_TANGENT_H

#include "approximation_helper.h"
#include "hyperbolic_trigonometric_function.h"

namespace Poincare {

class HyperbolicArcTangentNode final
    : public HyperbolicTrigonometricFunctionNode {
 public:
  constexpr static AliasesList k_functionName = "artanh";

  // PoolObject
  size_t size() const override { return sizeof(HyperbolicArcTangentNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "HyperbolicArcTangent";
  }
#endif

  // Properties
  Type otype() const override { return Type::HyperbolicArcTangent; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
};

class HyperbolicArcTangent final
    : public ExpressionOneChild<HyperbolicArcTangent, HyperbolicArcTangentNode,
                                HyperbolicTrigonometricFunction> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
