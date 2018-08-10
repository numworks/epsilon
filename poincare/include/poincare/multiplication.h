#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/approximation_helper.h>
#include <poincare/n_ary_expression_node.h>

namespace Poincare {

class MultiplicationNode : public NAryExpressionNode {
/*  friend class Addition;
  friend class Division;
  friend class Logarithm;
  friend class Opposite;
  friend class Power;
  friend class Subtraction;
  friend class Symbol;
  friend class Complex<float>;
  friend class Complex<double>;*/
public:
  using NAryExpressionNode::NAryExpressionNode;

  // Tree
  static MultiplicationNode * FailedAllocationStaticNode();
  MultiplicationNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  size_t size() const override { return sizeof(MultiplicationNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Multiplication";
  }
#endif

  Type type() const override { return Type::Multiplication; }
  Sign sign() const override;
  int polynomialDegree(char symbolName) const override;
  int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const override;

  // Evaluation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d) { return Complex<T>(c*d); }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  template<typename T> static void computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n);

private:
  // Property
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const override;

  // Layout
  bool needsParenthesesWithParent(const SerializationHelperInterface * parentNode) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const override;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override;

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

class Multiplication : public NAryExpression {
public:
  Multiplication(const MultiplicationNode * n) : NAryExpression(n) {}
  Multiplication() : NAryExpression(TreePool::sharedPool()->createTreeNode<MultiplicationNode>()) {}
  Multiplication(Expression e1, Expression e2) : Multiplication() {
    addChildAtIndexInPlace(e1, 0, 0);
    addChildAtIndexInPlace(e1, 0, numberOfChildren());
  }

  // Expression
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const;
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const;
  Expression shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const;
private:
  // Simplification
  Expression privateShallowReduce(Context& context, Preferences::AngleUnit angleUnit, bool expand, bool canBeInterrupted) const;
  void mergeMultiplicationOperands();
  void factorizeBase(Expression e1, Expression e2, Context & context, Preferences::AngleUnit angleUnit);
  void factorizeExponent(Expression e1, Expression e2, Context & context, Preferences::AngleUnit angleUnit);
  Expression distributeOnOperandAtIndex(int index, Context & context, Preferences::AngleUnit angleUnit);
  void addMissingFactors(Expression factor, Context & context, Preferences::AngleUnit angleUnit);
  void factorizeSineAndCosine(Expression o1, Expression o2, Context & context, Preferences::AngleUnit angleUnit);
  static bool HaveSameNonRationalFactors(const Expression e1, const Expression e2);
  static bool TermsHaveIdenticalBase(const Expression e1, const Expression e2);
  static bool TermsHaveIdenticalExponent(const Expression e1, const Expression e2);
  static bool TermHasRationalBase(const Expression e);
  static bool TermHasRationalExponent(const Expression e);
  static const Expression CreateExponent(Expression e);
  /* Warning: mergeNegativePower doesnot always return  a multiplication:
   *      *(b^-1,c^-1) -> (bc)^-1 */
  Expression mergeNegativePower(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
