#ifndef POINCARE_FACTORIAL_H
#define POINCARE_FACTORIAL_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>

namespace Poincare {

class Factorial : public StaticHierarchy<1> {
public:
  Factorial(const Expression * argument, bool clone = true);
  Type type() const override;
  Expression * clone() const override;
private:
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit);
  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
  return EvaluationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  int compareToGreaterTypeExpression(const Expression * e) const override;
  int compareToSameTypeExpression(const Expression * e) const override;
};

}

#endif
