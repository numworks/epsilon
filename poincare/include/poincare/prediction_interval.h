#ifndef POINCARE_PREDICTION_INTERVAL_H
#define POINCARE_PREDICTION_INTERVAL_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>

namespace Poincare {

class PredictionInterval : public StaticHierarchy<2>  {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  int polynomialDegree(char symbolName) const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  const char * name() const { return "prediction95"; }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Expression * privateApproximate(Expression::SinglePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(Expression::DoublePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Expression * templatedApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif

