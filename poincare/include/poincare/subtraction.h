#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class SubtractionNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(SubtractionNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Subtraction";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::Subtraction; }
  int polynomialDegree(Context & context, const char * symbolName) const override;

  // Approximation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d) { return Complex<T>(c - d); }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }

  /* Layout */
  bool childNeedsParenthesis(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;

private:
  /* Evaluation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
    return ApproximationHelper::ElementWiseOnComplexMatrices(m, n, compute<T>);
  }
};

class Subtraction final : public Expression {
public:
  Subtraction();
  Subtraction(const SubtractionNode * n) : Expression(n) {}
  Subtraction(Expression child0, Expression child1) : Subtraction() {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
};

}

#endif
