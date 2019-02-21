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

  // Complex
  bool isReal(Context & context) const override;

  // Properties
  Type type() const override { return Type::Power; }
  Sign sign(Context * context) const override;
  Expression setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;

  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat);

private:
  constexpr static int k_maxApproximatePowerMatrix = 1000;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  bool childNeedsParenthesis(const TreeNode * child) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplify
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const override;
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const override;
  Expression denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override;
  // Evaluation
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> d, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, complexFormat, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, complexFormat, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

class Power final : public Expression {
  friend class PowerNode;
  friend class Round;
public:
  Power(const PowerNode * n) : Expression(n) {}
  static Power Builder(Expression base, Expression exponent) { return TreeHandle::FixedArityBuilder<Power, PowerNode>(ArrayBuilder<TreeHandle>(base, exponent).array(), 2); }

  Expression setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);

private:
  constexpr static int k_maxExactPowerMatrix = 100;
  constexpr static int k_maxNumberOfTermsInExpandedMultinome = 25;

  // Simplification
  Expression denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

  Expression simplifyPowerPower(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression simplifyPowerMultiplication(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression simplifyRationalRationalPower(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);

  static Expression CreateSimplifiedIntegerRationalPower(Integer i, Rational r, bool isDenominator, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression removeSquareRootsFromDenominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  bool parentIsALogarithmOfSameBase() const;
  bool isNthRootOfUnity() const;
  Expression equivalentExpressionUsingStandardExpression() const;
  static Expression CreateComplexExponent(const Expression & r, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target); // Returns e^(i*pi*r)
  static bool TermIsARationalSquareRootOrRational(const Expression& e);
  static const Rational RadicandInExpression(const Expression & e);
  static const Rational RationalFactorInExpression(const Expression & e);
  static bool RationalExponentShouldNotBeReduced(const Rational & b, const Rational & r);
};

}

#endif
