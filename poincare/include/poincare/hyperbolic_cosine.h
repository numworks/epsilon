#ifndef POINCARE_HYPERBOLIC_COSINE_H
#define POINCARE_HYPERBOLIC_COSINE_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicCosineNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicCosineNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicCosine";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicCosine; }
private:
  const char * name() const override { return "cosh"; }
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class HyperbolicCosine final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicCosine() : HyperbolicTrigonometricFunction(TreePool::sharedPool()->createTreeNode<HyperbolicCosineNode>()) {}
  HyperbolicCosine(const HyperbolicCosineNode * n) : HyperbolicTrigonometricFunction(n) {}
  explicit HyperbolicCosine(Expression operand) : HyperbolicCosine() {
    replaceChildAtIndexInPlace(0, operand);
  }
};

}

#endif
