#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/serialization_helper.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class SineNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(SineNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Sine";
  }
#endif

  // Properties
  Type type() const override { return Type::Sine; }
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "sin"; }

  // Simplication
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class Sine final : public Expression {
public:
  Sine();
  Sine(const SineNode * n) : Expression(n) {}
  explicit Sine(Expression operand) : Sine() {
    replaceChildAtIndexInPlace(0, operand);
  }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
