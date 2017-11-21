#ifndef POINCARE_OPPOSITE_H
#define POINCARE_OPPOSITE_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/evaluation_engine.h>

namespace Poincare {

class Opposite : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Expression * clone() const override;
  Type type() const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, AngleUnit angleUnit);
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Expression * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::map<float>(this, context, angleUnit, compute<float>);
  }
  Expression * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::map<double>(this, context, angleUnit, compute<double>);
  }
};

}

#endif
