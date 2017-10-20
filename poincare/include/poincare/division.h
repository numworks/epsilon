#ifndef POINCARE_DIVISION_H
#define POINCARE_DIVISION_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class Division : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  /* Simplification */
  Expression * immediateSimplify(Context& context, AngleUnit angleUnit) override;

  Expression * immediateBeautify(Context & context, AngleUnit angleUnit) override;
private:
  template<typename T> static Evaluation<T> * computeOnMatrixAndComplex(Evaluation<T> * m, const Complex<T> * c) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Evaluation<T> * computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n);
  template<typename T> static Evaluation<T> * computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n);

  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }

  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, name());
  }
  static const char * name() { return "/"; }
  Expression * factorOfTypeInOperand(Type type, int operandIndex, int k);
};

}

#endif
