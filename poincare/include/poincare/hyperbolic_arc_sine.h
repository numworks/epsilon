#ifndef POINCARE_HYPERBOLIC_ARC_SINE_H
#define POINCARE_HYPERBOLIC_ARC_SINE_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicArcSineNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicArcSineNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicArcSine";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicArcSine; }
private:
  const char * name() const override { return "asinh"; }
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class HyperbolicArcSine final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicArcSine() : HyperbolicTrigonometricFunction(TreePool::sharedPool()->createTreeNode<HyperbolicArcSineNode>()) {}
  HyperbolicArcSine(const HyperbolicArcSineNode * n) : HyperbolicTrigonometricFunction(n) {}
  explicit HyperbolicArcSine(Expression operand) : HyperbolicArcSine() {
    replaceChildAtIndexInPlace(0, operand);
  }
};

}


#endif
