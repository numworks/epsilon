#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/approximation_helper.h>
#include <poincare/allocation_failure_layout_node.h>
#include <poincare/layout_helper.h>
#include <poincare/n_ary_expression_node.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class AdditionNode : public NAryExpressionNode {
  friend class Addition;
/* TODO friend class Logarithm;
  friend class Multiplication;
  friend class Subtraction;
  friend class Power;
  friend class Complex<float>;
  friend class Complex<double>;*/
public:
  using NAryExpressionNode::NAryExpressionNode;

  // Tree
  static AdditionNode * FailedAllocationStaticNode();
  AdditionNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  size_t size() const override { return sizeof(AdditionNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Addition";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::Addition; }
  int polynomialDegree(char symbolName) const override;
  int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const override;

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
  bool needsParenthesesWithParent(const SerializationHelperInterface * parentNode) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  static const char * name() { return "+"; }

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
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

class Addition : public NAryExpression {
public:
  Addition(const AdditionNode * n) : NAryExpression(n) {}
  Addition() : NAryExpression(TreePool::sharedPool()->createTreeNode<AdditionNode>()) {}
  Addition(Expression e1, Expression e2) : Addition() {
    addChildAtIndexInPlace(e2, 0, 0);
    addChildAtIndexInPlace(e1, 0, numberOfChildren());
  }
  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
  int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const;
private:
  static const Number NumeralFactor(const Expression & e);
  static bool TermsHaveIdenticalNonNumeralFactors(const Expression & e1, const Expression & e2);
  Expression factorizeOnCommonDenominator(Context & context, Preferences::AngleUnit angleUnit) const;
  void factorizeChildrenAtIndexesInPlace(int index1, int index2, Context & context, Preferences::AngleUnit angleUnit);
  AdditionNode * node() const { return static_cast<AdditionNode *>(Expression::node()); }
};

}

#endif
