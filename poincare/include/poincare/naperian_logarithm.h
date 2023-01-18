#ifndef POINCARE_NAPERIAN_LOGARITHM_H
#define POINCARE_NAPERIAN_LOGARITHM_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class NaperianLogarithmNode final : public ExpressionNode  {
public:
  constexpr static AliasesList k_functionName = "ln";

  // TreeNode
  size_t size() const override { return sizeof(NaperianLogarithmNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NaperianLogarithm";
  }
#endif

  // Properties
  Type type() const override { return Type::NaperianLogarithm; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  /* Evaluation */
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
    /* ln has a branch cut on ]-inf, 0]: it is then multivalued on this cut. We
     * followed the convention chosen by the lib c++ of llvm on ]-inf+0i, 0+0i]
     * (warning: ln takes the other side of the cut values on ]-inf-0i, 0-0i]).
     * We manually handle the case where the argument is null, as the lib c++
     * gives log(0) = -inf, which is only a generous shorthand for the limit. */
    return Complex<T>::Builder(c == std::complex<T>(0) ? std::complex<T>(NAN, NAN) : std::log(c));
  }
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class NaperianLogarithm final : public ExpressionOneChild<NaperianLogarithm, NaperianLogarithmNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ReductionContext reductionContext);
};

}

#endif
