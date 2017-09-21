#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/evaluation_engine.h>

namespace Poincare {

class Subtraction : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  bool isCommutative() const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
private:
  template<typename T> static Evaluation<T> * computeOnMatrixAndComplex(Evaluation<T> * m, const Complex<T> * c) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Evaluation<T> * computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n);
  template<typename T> static Evaluation<T> * computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {
    return EvaluationEngine::elementWiseOnComplexMatrices(m, n, compute<T>);
  }

  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }

  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createInfixLayout(this, floatDisplayMode, complexFormat, "-");
  }
};

}

#endif
