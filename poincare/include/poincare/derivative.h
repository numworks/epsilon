#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/variable_context.h>

namespace Poincare {

class Derivative : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  int polynomialDegree(char symbolName) const override;
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "diff"; }
  /* Simplification */
  ExpressionReference shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> EvaluationReference<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T growthRateAroundAbscissa(T x, T h, Context & context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T riddersApproximation(Context & context, Preferences::AngleUnit angleUnit, T x, T h, T * error) const;
  // TODO: Change coefficients?
  constexpr static double k_maxErrorRateOnApproximation = 0.001;
  constexpr static double k_minInitialRate = 0.01;
  constexpr static double k_rateStepSize = 1.4;
};

}

#endif
