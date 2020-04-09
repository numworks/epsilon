#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/approximation_helper.h>
#include <poincare/n_ary_expression.h>
#include <poincare/rational.h>

namespace Poincare {

class AdditionNode final : public NAryExpressionNode {
  friend class Addition;
public:
  using NAryExpressionNode::NAryExpressionNode;

  // Tree
  size_t size() const override { return sizeof(AdditionNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Addition";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::Addition; }
  int polynomialDegree(Context * context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const override;

  // Evaluation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) { return Complex<T>::Builder(c+d); }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnComplexMatrices(m, n, complexFormat, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
    return MatrixComplex<T>::Undefined();
  }
  Expression getUnit() const override;

  // Simplification
  LayoutShape leftLayoutShape() const override {
    /* When beautifying a Multiplication of Additions, Parentheses are added
     * around Additions. As leftLayoutShape is called after beautifying, we
     * should never call it on an Addition. */
    assert(false);
    return NAryExpressionNode::leftLayoutShape();
  }
  LayoutShape rightLayoutShape() const override {
    assert(false);
    return NAryExpressionNode::rightLayoutShape();
  }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext reductionContext) override;

  /* Evaluation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return MatrixComplex<T>::Undefined();
  }
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, complexFormat, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
   }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, complexFormat, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
   }
};

class Addition final : public NAryExpression {
public:
  Addition(const AdditionNode * n) : NAryExpression(n) {}
  static Addition Builder(const Tuple & children = {}) {
    return TreeHandle::NAryBuilder<Addition, AdditionNode>(convert(children));
  }
  // TODO: Get rid of these two helper functions
  static Addition Builder(Expression e1) { return Addition::Builder({e1}); }
  static Addition Builder(Expression e1, Expression e2) { return Addition::Builder({e1, e2}); }
  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext reductionContext);
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const;
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order, Context * context, bool canBeInterrupted) {
    NAryExpression::sortChildrenInPlace(order, context, true, canBeInterrupted);
  }
private:
  static const Number NumeralFactor(const Expression & e);
  static inline int NumberOfNonNumeralFactors(const Expression & e);
  static inline const Expression FirstNonNumeralFactor(const Expression & e);

  static bool TermsHaveIdenticalNonNumeralFactors(const Expression & e1, const Expression & e2, Context * context);
  Expression factorizeOnCommonDenominator(ExpressionNode::ReductionContext reductionContext);
  void factorizeChildrenAtIndexesInPlace(int index1, int index2, ExpressionNode::ReductionContext reductionContext);
  AdditionNode * node() const { return static_cast<AdditionNode *>(Expression::node()); }
};

}

#endif
