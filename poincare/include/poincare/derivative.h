#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/parametered_expression.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>

namespace Poincare {

class DerivativeNode final : public ParameteredExpressionNode {
 public:
  // TreeNode
  size_t size() const override { return sizeof(DerivativeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Derivative";
  }
#endif

  // Properties
  Type type() const override { return Type::Derivative; }
  int polynomialDegree(Context* context, const char* symbolName) const override;

 private:
  bool isFirstOrder() const {
    return Expression::IsOne(Expression(childAtIndex(numberOfChildren() - 1)));
  }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
  template <typename T>
  T scalarApproximateWithValueForArgumentAndOrder(
      T evaluationArgument, int order,
      const ApproximationContext& approximationContext) const;
  template <typename T>
  T growthRateAroundAbscissa(
      T x, T h, int order,
      const ApproximationContext& approximationContext) const;
  template <typename T>
  T riddersApproximation(int order,
                         const ApproximationContext& approximationContext, T x,
                         T h, T* error) const;
  // TODO: Change coefficients?
  constexpr static double k_maxErrorRateOnApproximation = 0.001;
  constexpr static double k_minInitialRate = 0.01;
  constexpr static double k_rateStepSize = 1.4;
  constexpr static double k_minSignificantError = 3e-11;

  constexpr static int k_maxOrderForApproximation = 4;
};

class Derivative final : public ParameteredExpression {
 public:
  Derivative(const DerivativeNode* n) : ParameteredExpression(n) {}

  static Derivative Builder(Expression child0, Symbol child1,
                            Expression child2) {
    return TreeHandle::FixedArityBuilder<Derivative, DerivativeNode>(
        {child0, child1, child2, Rational::Builder(1)});
  }
  static Derivative Builder(Expression child0, Symbol child1, Expression child2,
                            Expression child3) {
    return TreeHandle::FixedArityBuilder<Derivative, DerivativeNode>(
        {child0, child1, child2, child3});
  }
  static Expression UntypedBuilder(Expression children);
  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper("diff", 4, &UntypedBuilder);

  constexpr static Expression::FunctionHelper s_functionHelperFirstOrder =
      Expression::FunctionHelper("diff", 3, &UntypedBuilder);

  constexpr static char k_defaultXNTChar = 'x';

  static void DerivateUnaryFunction(Expression function, Symbol symbol,
                                    Expression symbolValue,
                                    const ReductionContext& reductionContext);
  static Expression DefaultDerivate(Expression function,
                                    const ReductionContext& reductionContext,
                                    Symbol symbol);

  void deepReduceChildren(const ReductionContext& reductionContext);
  Expression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
