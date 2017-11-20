#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/evaluation_engine.h>

namespace Poincare {

class Multiplication : public DynamicHierarchy {
  using DynamicHierarchy::DynamicHierarchy;
  friend class Addition;
  friend class Division;
  friend class Logarithm;
  friend class Opposite;
  friend class Power;
  friend class Subtraction;
public:
  Type type() const override;
  Expression * clone() const override;
  Sign sign() const override;
  /* Evaluation */
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  template<typename T> static Matrix * computeOnComplexAndMatrix(const Complex<T> * c, const Matrix * m) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Matrix * computeOnMatrices(const Matrix * m, const Matrix * n);
private:
  /* Property */
  Expression * setSign(Sign s, Context & context, AngleUnit angleUnit) override;
  /* Layout */
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  Expression * privateShallowReduce(Context& context, AngleUnit angleUnit, bool expand);
  void factorizeBase(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit);
  void factorizeExponent(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit);
  Expression * distributeOnOperandAtIndex(int index, Context & context, AngleUnit angleUnit);
  Expression * cloneDenominator(Context & context, AngleUnit angleUnit) const override;
  void addMissingFactors(Expression * factor, Context & context, AngleUnit angleUnit);
  void factorizeSineAndCosine(Expression * o1, Expression * o2, Context & context, AngleUnit angleUnit);
  static bool HaveSameNonRationalFactors(const Expression * e1, const Expression * e2);
  static bool TermsHaveIdenticalBase(const Expression * e1, const Expression * e2);
  static bool TermsHaveIdenticalExponent(const Expression * e1, const Expression * e2);
  static bool TermHasRationalBase(const Expression * e);
  static bool TermHasIntegerExponent(const Expression * e);
  static bool TermHasRationalExponent(const Expression * e);
  static const Expression * CreateExponent(Expression * e);
  Expression * shallowBeautify(Context & context, AngleUnit angleUnit) override;
  // Warning: mergeNegativePower not always returns  a multiplication: *(b^-1,c^-1) -> (bc)^-1
  Expression * mergeNegativePower(Context & context, AngleUnit angleUnit);
  /* Evaluation */

  template<typename T> static Matrix * computeOnMatrixAndComplex(const Matrix * m, const Complex<T> * c) {
    return EvaluationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  Expression * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Expression * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

}

#endif
