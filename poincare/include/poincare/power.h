#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>

namespace Poincare {

class Power : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
  friend class Multiplication;
  friend class NthRoot;
  friend class SquareRoot;
public:
  Type type() const override;
  Expression * clone() const override;
  int sign() const override;
  Expression * turnIntoPositive(Context & context, AngleUnit angleUnit) override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  Expression * createDenominator(Context & context, AngleUnit angleUnit);
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
  Expression * shallowSimplify(Context& context, AngleUnit angleUnit) override;
  Expression * shallowBeautify(Context & context, AngleUnit angleUnit) override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, name());
  }
  static const char * name() { return "^"; }
  int compareToGreaterTypeExpression(const Expression * e) const override;
  int compareToSameTypeExpression(const Expression * e) const override;
  Expression * simplifyPowerPower(Power * p, Expression * r, Context & context, AngleUnit angleUnit);
  Expression * simplifyPowerMultiplication(Multiplication * m, Expression * r, Context & context, AngleUnit angleUnit);
  Expression * simplifyRationalRationalPower(Expression * result, Rational * a, Rational * b, Context & context, AngleUnit angleUnit);
  static Expression * CreateSimplifiedIntegerRationalPower(Integer i, Rational * r, bool isDenominator);
};

}

#endif
