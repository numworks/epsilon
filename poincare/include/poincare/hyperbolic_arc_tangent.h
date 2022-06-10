#ifndef POINCARE_HYPERBOLIC_ARC_TANGENT_H
#define POINCARE_HYPERBOLIC_ARC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicArcTangentNode final : public HyperbolicTrigonometricFunctionNode {
public:
  static constexpr char k_functionName[] = "atanh";

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicArcTangentNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicArcTangent";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicArcTangent; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};


class HyperbolicArcTangent final : public HandleOneChild<HyperbolicArcTangent, HyperbolicArcTangentNode, HyperbolicTrigonometricFunction> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
};

}

#endif
