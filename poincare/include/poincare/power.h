#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/approximation_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>

namespace Poincare {

class Power;

class PowerNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(PowerNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Power";
  }
#endif

  // Properties
  Type type() const override { return Type::Power; }
  Sign sign() const override;
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;

  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;

  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d);

private:
  constexpr static int k_maxApproximatePowerMatrix = 1000;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  bool childNeedsParenthesis(const TreeNode * child) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplify
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const override;
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override;
  // Evaluation
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n);
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> d);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

class Power final : public Expression {
  friend class PowerNode;
  friend class Round;
public:
  Power(Expression base, Expression exponent);
  Power(const PowerNode * n) : Expression(n) {}
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);

private:
  constexpr static int k_maxExactPowerMatrix = 100;
  constexpr static int k_maxNumberOfTermsInExpandedMultinome = 25;

  // Simplification
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const;

  Expression simplifyPowerPower(Context & context, Preferences::AngleUnit angleUnit);
  Expression simplifyPowerMultiplication(Context & context, Preferences::AngleUnit angleUnit);
  Expression simplifyRationalRationalPower(Context & context, Preferences::AngleUnit angleUnit);

  static Expression CreateSimplifiedIntegerRationalPower(Integer i, Rational r, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit);
  Expression removeSquareRootsFromDenominator(Context & context, Preferences::AngleUnit angleUnit);
  bool parentIsALogarithmOfSameBase() const;
  bool isNthRootOfUnity() const;
  static Expression CreateComplexExponent(const Expression & r); // Returns e^(i*pi*r)
  static bool TermIsARationalSquareRootOrRational(const Expression& e);
  static const Rational RadicandInExpression(const Expression & e);
  static const Rational RationalFactorInExpression(const Expression & e);
  static bool RationalExponentShouldNotBeReduced(const Rational & b, const Rational & r);
};

}

#endif
