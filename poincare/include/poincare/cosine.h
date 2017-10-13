#ifndef POINCARE_COSINE_H
#define POINCARE_COSINE_H

#include <poincare/trigonometrical_function.h>

namespace Poincare {

class Cosine : public TrigonometricalFunction  {
  using TrigonometricalFunction::TrigonometricalFunction;
public:
  Type type() const override;
  Expression * clone() const override;
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit = AngleUnit::Radian);
private:
  Trigonometry::Function trigonometricalFunctionType() const override {
    return Trigonometry::Function::Cosine;
  }
  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
  return EvaluationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, "cos");
  }
};

}

#endif
