#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/approximation_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class Power;

class PowerNode : public ExpressionNode {
public:
  // Allocation Failure
  static PowerNode * FailedAllocationStaticNode();
  PowerNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // TreeNode
  size_t size() const override { return sizeof(PowerNode); }
#if TREE_LOG
  const char * description() const override { return "Power";  }
#endif
  int numberOfChildren() const override { return 2; }

  // Properties
  Type type() const override { return Type::Power; }
  Sign sign() const override;
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const override;

  int polynomialDegree(char symbolName) const override;
  int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const override;

private:
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d);
  constexpr static int k_maxNumberOfTermsInExpandedMultinome = 25;
  constexpr static int k_maxExactPowerMatrix = 100;

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  bool needsParenthesesWithParent(const SerializationHelperInterface * parent) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  static const char * name() { return "^"; }

  /* Simplify */
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const override;
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override;
#if 0
   Expression * simplifyPowerPower(Power * p, Expression * r, Context & context, Preferences::AngleUnit angleUnit);
   Expression * simplifyPowerMultiplication(Multiplication * m, Expression * r, Context & context, Preferences::AngleUnit angleUnit);
  Expression * simplifyRationalRationalPower(Expression * result, Rational * a, Rational * b, Context & context, Preferences::AngleUnit angleUnit);
  Expression * removeSquareRootsFromDenominator(Context & context, Preferences::AngleUnit angleUnit);
  bool parentIsALogarithmOfSameBase() const;
  bool isNthRootOfUnity() const;
  static Expression * CreateSimplifiedIntegerRationalPower(Integer i, Rational * r, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit);
  static Expression * CreateNthRootOfUnity(const Rational r);
  static bool TermIsARationalSquareRootOrRational(const Expression * e);
  static const Rational * RadicandInExpression(const Expression * e);
  static const Rational * RationalFactorInExpression(const Expression * e);
  static bool RationalExponentShouldNotBeReduced(const Rational * b, const Rational * r);
#endif
  /* Evaluation */
  constexpr static int k_maxApproximatePowerMatrix = 1000;
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

class Power : public Expression {
public:
  Power(Expression base, Expression exponent) : Expression(TreePool::sharedPool()->createTreeNode<PowerNode>()) {
    replaceChildAtIndexInPlace(0, base);
    replaceChildAtIndexInPlace(1, exponent);
  }
  Power(const PowerNode * n) : Expression(n) {}
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const;
  int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const;
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) const;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const;

};


}

#endif
