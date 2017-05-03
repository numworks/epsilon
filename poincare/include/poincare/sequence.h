#ifndef POINCARE_SEQUENCE_H
#define POINCARE_SEQUENCE_H

#include <poincare/function.h>

namespace Poincare {

class Sequence : public Function {
public:
  Sequence(const char * name);
private:
  constexpr static float k_maxNumberOfSteps = 10000.0f;
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  virtual float emptySequenceValue() const = 0;
  virtual float approximateWithNextTerm(float sequence, float newTerm) const = 0;
  virtual ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const = 0;
  virtual Expression * evaluateWithNextTerm(Expression ** args, Context& context, AngleUnit angleUnit) const = 0;
};

}

#endif
