#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class IntegralNode final : public ParameteredExpressionNode {
 public:
  // TreeNode
  size_t size() const override { return sizeof(IntegralNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Integral";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::Integral; }
  int polynomialDegree(Context* context, const char* symbolName) const override;

 private:
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
    return LayoutShape::BoundaryPunctuation;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::MoreLetters;
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
  struct DetailedResult {
    T integral;
    T absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 20;
#ifdef LAGRANGE_METHOD
  template <typename T>
  T lagrangeGaussQuadrature(
      T a, T b, const ApproximationContext& approximationContext) const;
#else
  template <typename T>
  class Substitution {
   public:
    enum class Type { None, LeftOpen, RightOpen, RealLine };
    Type type;
    T originA;
    T originB;
  };
  class AlternativeIntegrand {
   public:
    double a;
    double b;
    Expression integrandNearA;
    Expression integrandNearB;
  };
  Expression rewriteIntegrandNear(
      Expression bound, const ReductionContext& reductionContext) const;
  template <typename T>
  T integrand(T x, Substitution<T> substitution,
              const ApproximationContext& approximationContext) const;
  template <typename T>
  T integrandNearBound(T x, T xc, AlternativeIntegrand alternativeIntegrand,
                       const ApproximationContext& approximationContext) const;
  template <typename T>
  DetailedResult<T> tanhSinhQuadrature(
      int level, AlternativeIntegrand alternativeIntegrand,
      const ApproximationContext& approximationContext) const;
  template <typename T>
  DetailedResult<T> kronrodGaussQuadrature(
      T a, T b, Substitution<T> substitution,
      const ApproximationContext& approximationContext) const;
  template <typename T>
  DetailedResult<T> adaptiveQuadrature(
      T a, T b, T eps, int numberOfIterations, Substitution<T> substitution,
      const ApproximationContext& approximationContext) const;
#endif
};

class Integral final : public ParameteredExpression {
 public:
  Integral(const IntegralNode* n) : ParameteredExpression(n) {}
  static Integral Builder(Expression child0, Symbol child1, Expression child2,
                          Expression child3) {
    return TreeHandle::FixedArityBuilder<Integral, IntegralNode>(
        {child0, child1, child2, child3});
  }
  static Expression UntypedBuilder(Expression children);

  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper("int", 4, &UntypedBuilder);
  constexpr static char k_defaultXNTChar = 'x';

  // Expression
  void deepReduceChildren(const ReductionContext& reductionContext);
  Expression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
