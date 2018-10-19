#ifndef POINCARE_HYPERBOLIC_SINE_H
#define POINCARE_HYPERBOLIC_SINE_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicSineNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicSineNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicSine";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicSine; }
private:
  const char * name() const override { return "sinh"; }
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class HyperbolicSine final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicSine() : HyperbolicTrigonometricFunction(TreePool::sharedPool()->createTreeNode<HyperbolicSineNode>()) {}
  HyperbolicSine(const HyperbolicSineNode * n) : HyperbolicTrigonometricFunction(n) {}
  explicit HyperbolicSine(Expression operand) : HyperbolicSine() {
    replaceChildAtIndexInPlace(0, operand);
  }
};

}

#endif
