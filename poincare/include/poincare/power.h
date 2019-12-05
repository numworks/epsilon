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
  bool isReal(Context * context) const;

  // Properties
  Type type() const override { return Type::Power; }
  Sign sign(Context * context) const override;
  Expression setSign(Sign s, ReductionContext reductionContext) override;
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;

  int polynomialDegree(Context * context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const override;

  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat);

private:
  constexpr static int k_maxApproximatePowerMatrix = 1000;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplify
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); }
  LayoutShape rightLayoutShape() const override { return LayoutShape::RightOfPower; }
  int simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const override;
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const override;
  Expression denominator(ReductionContext reductionContext) const override;
  // Evaluation
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> d, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, complexFormat, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, complexFormat, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

class Power final : public Expression {
  friend class PowerNode;
  friend class Round;
public:
  Power(const PowerNode * n) : Expression(n) {}
  static Power Builder(Expression base, Expression exponent) { return TreeHandle::FixedArityBuilder<Power, PowerNode>(ArrayBuilder<TreeHandle>(base, exponent).array(), 2); }

  Expression setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext);
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext reductionContext);

private:
  constexpr static int k_maxExactPowerMatrix = 100;
  constexpr static int k_maxNumberOfTermsInExpandedMultinome = 25;

  // Simplification
  Expression denominator(ExpressionNode::ReductionContext reductionContext) const;

  Expression simplifyPowerPower(ExpressionNode::ReductionContext reductionContext);
  Expression simplifyPowerMultiplication(ExpressionNode::ReductionContext reductionContext);
  Expression simplifyRationalRationalPower(ExpressionNode::ReductionContext reductionContext);

  static Expression CreateSimplifiedIntegerRationalPower(Integer i, Rational r, bool isDenominator, ExpressionNode::ReductionContext reductionContext);
  Expression removeSquareRootsFromDenominator(ExpressionNode::ReductionContext reductionContext);
  bool parentIsALogarithmOfSameBase() const;
  bool isNthRootOfUnity() const;
  Expression equivalentExpressionUsingStandardExpression() const;
  static Expression CreateComplexExponent(const Expression & r, ExpressionNode::ReductionContext reductionContext); // Returns e^(i*pi*r)
  static bool TermIsARationalSquareRootOrRational(const Expression& e);
  static const Rational RadicandInExpression(const Expression & e);
  static const Rational RationalFactorInExpression(const Expression & e);
  static bool RationalExponentShouldNotBeReduced(const Rational & b, const Rational & r);
};

}

#endif
