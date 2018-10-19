#ifndef POINCARE_NAPERIAN_LOGARITHM_H
#define POINCARE_NAPERIAN_LOGARITHM_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class NaperianLogarithmNode final : public ExpressionNode  {
public:
  // TreeNode
  size_t size() const override { return sizeof(NaperianLogarithmNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "NaperianLogarithm";
  }
#endif

  // Properties
  Type type() const override { return Type::NaperianLogarithm; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  static const char * name() { return "ln"; }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
    /* ln has a branch cut on ]-inf, 0]: it is then multivalued on this cut. We
     * followed the convention chosen by the lib c++ of llvm on ]-inf+0i, 0+0i]
     * (warning: ln takes the other side of the cut values on ]-inf-0i, 0-0i]). */
    return Complex<T>(std::log(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class NaperianLogarithm final : public Expression {
public:
  NaperianLogarithm();
  NaperianLogarithm(const NaperianLogarithmNode * n) : Expression(n) {}
  explicit NaperianLogarithm(Expression operand) : NaperianLogarithm() {
    replaceChildAtIndexInPlace(0, operand);
  }
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
