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
  int polynomialDegree(Context * context, const char * symbolName) const override;
  bool childNeedsUserParentheses(const Expression & child) const override;

  // Approximation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) { return Complex<T>::Builder(c - d); }
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, complexFormat, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, complexFormat, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Simplification */
  Expression shallowReduce(ReductionContext reductionContext) override;

private:
  /* Evaluation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnComplexMatrices(m, n, complexFormat, compute<T>);
  }
};

class Subtraction final : public Expression {
public:
  Subtraction(const SubtractionNode * n) : Expression(n) {}
  static Subtraction Builder() { return TreeHandle::FixedArityBuilder<Subtraction, SubtractionNode>(); }
  static Subtraction Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Subtraction, SubtractionNode>(ArrayBuilder<TreeHandle>(child0, child1).array(), 2); }

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

};

}

#endif
