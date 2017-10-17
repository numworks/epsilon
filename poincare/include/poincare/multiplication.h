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
  void turnIntoPositive() override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  template<typename T> static Evaluation<T> * computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * m) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Evaluation<T> * computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n);

  static bool HaveSameNonRationalFactors(const Expression * e1, const Expression * e2);
  /* Simplification */
  Expression * immediateSimplify() override;
  Expression * createDenominator();
  void leastCommonMultiple(Expression * factor);
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
    return LayoutEngine::createInfixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, name());
  }
  const char * name() const { return "*"; }
  /* Simplification */
  void factorize();
  void factorizeBase(Expression * e1, Expression * e2);
  void factorizeExponent(Expression * e1, Expression * e2);
  Expression * distributeOnChildAtIndex(int index);
  static bool TermsHaveIdenticalBase(const Expression * e1, const Expression * e2);
  static bool TermsHaveIdenticalNonUnitaryExponent(const Expression * e1, const Expression * e2);
  static bool TermHasRationalBase(const Expression * e);
  static bool TermHasIntegerExponent(const Expression * e);
  static const Expression * CreateExponent(Expression * e);
  bool isUselessOperand(const Rational * r) override;
  // Warning: mergeNegativePower not always returns  a multiplication: *(b^-1,c^-1) -> (bc)^-1
  Expression * immediateBeautify() override;
  Expression * mergeNegativePower();
};

}

#endif
