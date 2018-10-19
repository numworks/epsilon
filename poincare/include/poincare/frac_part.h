#ifndef POINCARE_FRAC_PART_H
#define POINCARE_FRAC_PART_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class FracPartNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(FracPartNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "FracPart";
  }
#endif

  // Properties
  Type type() const override { return Type::FracPart; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  const char * name() const { return "frac"; }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class FracPart final : public Expression {
public:
  FracPart();
  FracPart(const FracPartNode * n) : Expression(n) {}
  explicit FracPart(Expression operand) : FracPart() {
    replaceChildAtIndexInPlace(0, operand);
  }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
