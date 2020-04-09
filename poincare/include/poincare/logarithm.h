#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/addition.h>
#include <poincare/complex.h>

namespace Poincare {

template<int T>
class LogarithmNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(LogarithmNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Logarithm";
  }
#endif

  // Properties
  Type type() const override { return Type::Logarithm; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext) override;
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  template<typename U> static Complex<U> computeOnComplex(const std::complex<U> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
    /* log has a branch cut on ]-inf, 0]: it is then multivalued on this cut. We
     * followed the convention chosen by the lib c++ of llvm on ]-inf+0i, 0+0i]
     * (warning: log takes the other side of the cut values on ]-inf-0i, 0-0i]). */
    return Complex<U>::Builder(std::log10(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename U> Evaluation<U> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Logarithm final : public Expression {
  friend class LogarithmNode<2>;
public:
  Logarithm(const LogarithmNode<2> * n) : Expression(n) {}
  static Logarithm Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Logarithm, LogarithmNode<2>>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("log", 2, &UntypedBuilderTwoChildren<Logarithm>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify();

private:
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext);
  Expression simpleShallowReduce(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Integer simplifyLogarithmIntegerBaseInteger(Integer i, Integer & base, Addition & a, bool isDenominator);
  Expression splitLogarithmInteger(Integer i, bool isDenominator, ExpressionNode::ReductionContext reductionContext);
  bool parentIsAPowerOfSameBase() const;
};

class CommonLogarithm : public Expression {
public:
  CommonLogarithm(const LogarithmNode<1> * n) : Expression(n) {}
  static CommonLogarithm Builder(Expression child) { return TreeHandle::FixedArityBuilder<CommonLogarithm, LogarithmNode<1>>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("log", 1, &UntypedBuilderOneChild<CommonLogarithm>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
