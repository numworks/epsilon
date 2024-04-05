#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class IntegralNode final : public ParameteredExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(IntegralNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Integral";
  }
#endif

  // ExpressionNode
  Type otype() const override { return Type::Integral; }
  int polynomialDegree(Context* context, const char* symbolName) const override;

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
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
  template <typename T>
  static bool DetailedResultIsValid(DetailedResult<T> result);
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
    OExpression integrandNearA;
    OExpression integrandNearB;
  };
  OExpression rewriteIntegrandNear(
      OExpression bound, const ReductionContext& reductionContext) const;
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
  template <typename T>
  DetailedResult<T> iterateAdaptiveQuadrature(
      DetailedResult<T> quadKG, T a, T b, T eps, int numberOfIterations,
      Substitution<T> substitution,
      const ApproximationContext& approximationContext) const;
#endif
};

class Integral final : public ParameteredExpression {
 public:
  Integral(const IntegralNode* n) : ParameteredExpression(n) {}
  static Integral Builder(OExpression child0, Symbol child1, OExpression child2,
                          OExpression child3) {
    return PoolHandle::FixedArityBuilder<Integral, IntegralNode>(
        {child0, child1, child2, child3});
  }
  static OExpression UntypedBuilder(OExpression children);

  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("int", 4, &UntypedBuilder);
  constexpr static char k_defaultXNTChar = 'x';

  // OExpression
  void deepReduceChildren(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
