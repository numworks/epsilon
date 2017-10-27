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
  int sign() const override;
  Expression * turnIntoPositive(Context & context, AngleUnit angleUnit) override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  template<typename T> static Evaluation<T> * computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * m) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Evaluation<T> * computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n);

  static bool HaveSameNonRationalFactors(const Expression * e1, const Expression * e2);
  /* Simplification */
  Expression * immediateSimplify(Context& context, AngleUnit angleUnit) override;

  Expression * createDenominator(Context & context, AngleUnit angleUnit);
  void leastCommonMultiple(Expression * factor, Context & context, AngleUnit angleUnit);
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
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  /* Simplification */
  void factorize(Context & context, AngleUnit angleUnit);
  bool resolveSquareRootAtDenominator(Context & context, AngleUnit angleUnit);
  void factorizeBase(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit);
  void factorizeExponent(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit);
  Expression * distributeOnChildAtIndex(int index, Context & context, AngleUnit angleUnit);
  static bool TermsHaveIdenticalBase(const Expression * e1, const Expression * e2);
  static bool TermsHaveIdenticalNonUnitaryExponent(const Expression * e1, const Expression * e2);
  static bool TermHasRationalBase(const Expression * e);
  static bool TermHasIntegerExponent(const Expression * e);
  static bool TermIsARationalSquareRootOrRational(const Expression * e);
  static const Rational * RadicandInExpression(const Expression * e);
  static const Rational * RationalFactorInExpression(const Expression * e);
  static const Expression * CreateExponent(Expression * e);
  bool isUselessOperand(const Rational * r) override;
  // Warning: mergeNegativePower not always returns  a multiplication: *(b^-1,c^-1) -> (bc)^-1
  Expression * immediateBeautify(Context & context, AngleUnit angleUnit) override;
  Expression * mergeNegativePower(Context & context, AngleUnit angleUnit);
};

}

#endif
