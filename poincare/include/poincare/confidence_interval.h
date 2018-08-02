#ifndef POINCARE_CONFIDENCE_INTERVAL_H
#define POINCARE_CONFIDENCE_INTERVAL_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>

namespace Poincare {

class ConfidenceInterval : public StaticHierarchy<2> {
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
  virtual const char * name() const { return "confidence"; }
  /* Simplification */
  ExpressionReference shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  EvaluationReference<float> approximate(Expression::SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(Expression::DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> EvaluationReference<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class SimplePredictionInterval : public ConfidenceInterval {
private:
  const char * name() const { return "prediction"; }
};

}

#endif

