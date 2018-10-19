#ifndef POINCARE_HYPERBOLIC_ARC_TANGENT_H
#define POINCARE_HYPERBOLIC_ARC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicArcTangentNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicArcTangentNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicArcTangent";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicArcTangent; }
private:
  const char * name() const override { return "atanh"; }
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class HyperbolicArcTangent final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicArcTangent() : HyperbolicTrigonometricFunction(TreePool::sharedPool()->createTreeNode<HyperbolicArcTangentNode>()) {}
  HyperbolicArcTangent(const HyperbolicArcTangentNode * n) : HyperbolicTrigonometricFunction(n) {}
  explicit HyperbolicArcTangent(Expression operand) : HyperbolicArcTangent() {
    replaceChildAtIndexInPlace(0, operand);
  }
};

}

#endif
