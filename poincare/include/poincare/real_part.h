#ifndef POINCARE_REEL_PART_H
#define POINCARE_REEL_PART_H

#include <poincare/layout_helper.h>
#include <poincare/expression.h>
#include <poincare/serialization_helper.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class RealPartNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(RealPartNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "RealPart";
  }
#endif

  // Properties
  Type type() const override { return Type::RealPart; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "re"; }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
    return Complex<T>(std::real(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class RealPart final : public Expression {
public:
  RealPart();
  RealPart(const RealPartNode * n) : Expression(n) {}
  explicit RealPart(Expression operand) : RealPart() {
    replaceChildAtIndexInPlace(0, operand);
  }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
