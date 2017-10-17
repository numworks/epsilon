#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>

namespace Poincare {

class Power : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  int sign() const override;
  void turnIntoPositive() override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  /* Simplification */
  Expression * immediateSimplify() override;
  Expression * createDenominator();
private:
  constexpr static float k_maxNumberOfSteps = 10000.0f;
  template<typename T> static Evaluation<T> * computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n);
  template<typename T> static Evaluation<T> * computeOnMatrixAndComplex(Evaluation<T> * m, const Complex<T> * d);
  template<typename T> static Evaluation<T> * computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n);

  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }

  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, "^");
  }

  int compareToGreaterTypeExpression(const Expression * e) const override;
  int compareToSameTypeExpression(const Expression * e) const override;
  /* Simplification */
  Expression * immediateBeautify() override;
  Expression * simplifyPowerPower(Power * p, Expression * r);
  Expression * simplifyPowerMultiplication(Multiplication * m, Expression * r);
  Expression * simplifyRationalRationalPower(Expression * result, Rational * a, Rational * b);
  static Expression * CreateSimplifiedIntegerRationalPower(Integer i, Rational * r, bool isDenominator);
};

}

#endif
