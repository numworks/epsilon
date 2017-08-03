#ifndef POINCARE_SEQUENCE_H
#define POINCARE_SEQUENCE_H

#include <poincare/function.h>

namespace Poincare {

class Sequence : public Function {
public:
  Sequence(const char * name);
private:
  constexpr static float k_maxNumberOfSteps = 10000.0f;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Evaluation * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  virtual float emptySequenceValue() const = 0;
  virtual ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const = 0;
  virtual Evaluation * evaluateWithNextTerm(Evaluation * a, Evaluation * b) const = 0;
};

}

#endif
