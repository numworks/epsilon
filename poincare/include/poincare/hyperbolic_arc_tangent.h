#ifndef POINCARE_HYPERBOLIC_ARC_TANGENT_H
#define POINCARE_HYPERBOLIC_ARC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_arc_trigonometric_function.h>

namespace Poincare {

class HyperbolicArcTangentNode : public HyperbolicArcTrigonometricFunctionNode {
public:
  // Allocation Failure
  static HyperbolicArcTangentNode * FailedAllocationStaticNode();
  HyperbolicArcTangentNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

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

class HyperbolicArcTangent : public HyperbolicArcTrigonometricFunction {
public:
  HyperbolicArcTangent() : HyperbolicArcTrigonometricFunction(TreePool::sharedPool()->createTreeNode<HyperbolicArcTangentNode>()) {}
  HyperbolicArcTangent(const HyperbolicArcTangentNode * n) : HyperbolicArcTrigonometricFunction(n) {}
  HyperbolicArcTangent(Expression operand) : HyperbolicArcTangent() {
    replaceChildAtIndexInPlace(0, operand);
  }
};

}

#endif
