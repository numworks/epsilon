#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/evaluation_engine.h>

namespace Poincare {

class Multiplication : public DynamicHierarchy {
  using DynamicHierarchy::DynamicHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  template<typename T> static Evaluation<T> * computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * m) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Evaluation<T> * computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n);

  static bool HaveSameNonRationalFactors(const Expression * e1, const Expression * e2);
private:
  template<typename T> static Evaluation<T> * computeOnMatrixAndComplex(Evaluation<T> * m, const Complex<T> * c) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createInfixLayout(this, floatDisplayMode, complexFormat, "*");
  }
  /* Simplification */
  void privateSimplify() override;
  void factorizeChildren(Expression * e1, Expression * e2);
  static bool TermsHaveIdenticalBase(const Expression * e1, const Expression * e2);
  static const Expression * CreateExponent(Expression * e);
};

}

#endif
