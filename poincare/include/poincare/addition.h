#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/approximation_helper.h>
#include <poincare/array_builder.h>
#include <poincare/n_ary_expression_node.h>
#include <poincare/rational.h>

namespace Poincare {

class AdditionNode final : public NAryExpressionNode {
  friend class Addition;
public:
  using NAryExpressionNode::NAryExpressionNode;

  // Tree
  size_t size() const override { return sizeof(AdditionNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Addition";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::Addition; }
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;

  // Evaluation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) { return Complex<T>(c+d); }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnComplexMatrices(m, n, complexFormat, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
private:
  // Layout
  bool childNeedsParenthesis(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  /* Evaluation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, complexFormat, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
   }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, complexFormat, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
   }
};

class Addition final : public NAryExpression {
public:
  Addition(const AdditionNode * n) : NAryExpression(n) {}
  static Addition Builder() { return Addition(); }
  static Addition Builder(Expression e1) { return Addition::Builder(&e1, 1); }
  static Addition Builder(Expression e1, Expression e2) { return Addition::Builder(ArrayBuilder<Expression>(e1, e2).array(), 2); }
  static Addition Builder(Expression * children, size_t numberOfChildren) {
    Addition a = Addition::Builder();
    for (size_t i = 0; i < numberOfChildren; i++) {
      a.addChildAtIndexInPlace(children[i], i, i);
    }
    return a;
  }
  // Expression
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
private:
  Addition() : NAryExpression(TreePool::sharedPool()->createTreeNode<AdditionNode>()) {}

  static const Number NumeralFactor(const Expression & e);
  static inline int NumberOfNonNumeralFactors(const Expression & e);
  static inline const Expression FirstNonNumeralFactor(const Expression & e);

  static bool TermsHaveIdenticalNonNumeralFactors(const Expression & e1, const Expression & e2);
  Expression factorizeOnCommonDenominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  void factorizeChildrenAtIndexesInPlace(int index1, int index2, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  AdditionNode * node() const { return static_cast<AdditionNode *>(Expression::node()); }
};

}

#endif
