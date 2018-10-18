#ifndef POINCARE_HYPERBOLIC_ARC_COSINE_H
#define POINCARE_HYPERBOLIC_ARC_COSINE_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicArcCosineNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicArcCosineNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicArcCosine";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicArcCosine; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class HyperbolicArcCosine final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicArcCosine() : HyperbolicTrigonometricFunction(TreePool::sharedPool()->createTreeNode<HyperbolicArcCosineNode>()) {}
  HyperbolicArcCosine(const HyperbolicArcCosineNode * n) : HyperbolicTrigonometricFunction(n) {}
  explicit HyperbolicArcCosine(Expression operand) : HyperbolicArcCosine() {
    replaceChildAtIndexInPlace(0, operand);
  }
  static const char * Name() { return "acosh"; }
};

}

#endif
