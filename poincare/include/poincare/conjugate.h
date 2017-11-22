#ifndef POINCARE_CONJUGATE_H
#define POINCARE_CONJUGATE_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class Conjugate : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, "conj");
  }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit);
  Expression * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Expression * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

}

#endif

