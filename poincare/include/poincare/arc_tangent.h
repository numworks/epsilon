#ifndef POINCARE_ARC_TANGENT_H
#define POINCARE_ARC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/serialization_helper.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class ArcTangentNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ArcTangentNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ArcTangent";
  }
#endif

  // Properties
  Type type() const override { return Type::ArcTangent; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "atan"; }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class ArcTangent final : public Expression {
public:
  ArcTangent();
  ArcTangent(const ArcTangentNode * n) : Expression(n) {}
  explicit ArcTangent(Expression operand) : ArcTangent() {
    replaceChildAtIndexInPlace(0, operand);
  }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
