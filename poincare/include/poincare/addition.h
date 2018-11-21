#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/approximation_helper.h>
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
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d) { return Complex<T>(c+d); }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
    return ApproximationHelper::ElementWiseOnComplexMatrices(m, n, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
private:
  // Layout
  bool childNeedsParenthesis(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;

  /* Evaluation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
   }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
   }
};

class Addition final : public NAryExpression {
public:
  Addition(const AdditionNode * n) : NAryExpression(n) {}
  Addition();
  explicit Addition(Expression e1) : Addition() {
    addChildAtIndexInPlace(e1, 0, 0);
  }
  Addition(Expression e1, Expression e2) : Addition() {
    addChildAtIndexInPlace(e2, 0, 0);
    addChildAtIndexInPlace(e1, 0, numberOfChildren());
  }
  Addition(Expression * children, size_t numberOfChildren) : Addition() {
    for (size_t i = 0; i < numberOfChildren; i++) {
      addChildAtIndexInPlace(children[i], i, i);
    }
  }
  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
private:
  static const Number NumeralFactor(const Expression & e);
  static inline int NumberOfNonNumeralFactors(const Expression & e);
  static inline const Expression FirstNonNumeralFactor(const Expression & e);

  static bool TermsHaveIdenticalNonNumeralFactors(const Expression & e1, const Expression & e2);
  Expression factorizeOnCommonDenominator(Context & context, Preferences::AngleUnit angleUnit);
  void factorizeChildrenAtIndexesInPlace(int index1, int index2, Context & context, Preferences::AngleUnit angleUnit);
  AdditionNode * node() const { return static_cast<AdditionNode *>(Expression::node()); }
};

}

#endif
