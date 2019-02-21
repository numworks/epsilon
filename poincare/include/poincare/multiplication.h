#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/approximation_helper.h>
#include <poincare/array_builder.h>
#include <poincare/n_ary_expression_node.h>

namespace Poincare {

class MultiplicationNode /*final*/ : public NAryExpressionNode {
  friend class Addition;
public:
  using NAryExpressionNode::NAryExpressionNode;

  // Tree
  size_t size() const override { return sizeof(MultiplicationNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Multiplication";
  }
#endif

  // Properties
  Type type() const override { return Type::Multiplication; }
  Sign sign(Context * context) const override;
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;

  // Approximation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) { return Complex<T>::Builder(c*d); }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);

private:
  // Property
  Expression setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  // Layout
  bool childNeedsParenthesis(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  Expression denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override;

  /* Approximation */
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

class Multiplication final : public NAryExpression {
  friend class AdditionNode;
  friend class Addition;
  friend class Power;
public:
  Multiplication(const MultiplicationNode * n) : NAryExpression(n) {}
  static Multiplication Builder();
  static Multiplication Builder(Expression e1) { return Multiplication::Builder(&e1, 1); }
  static Multiplication Builder(Expression e1, Expression e2) { return Multiplication::Builder(ArrayBuilder<Expression>(e1, e2).array(), 2); }
  static Multiplication Builder(Expression e1, Expression e2, Expression e3) { return Multiplication::Builder(ArrayBuilder<Expression>(e1, e2, e3).array(), 3); }

  template<typename T> static void computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);
  // Expression
  Expression setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
  Expression denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
private:
  // Constructors
  static Multiplication Builder(Expression * children, size_t numberOfChildren);

  // Simplification
  Expression privateShallowReduce(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool expand, bool canBeInterrupted);
  void mergeMultiplicationChildrenInPlace();
  void factorizeBase(int i, int j, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  void mergeInChildByFactorizingBase(int i, Expression e, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  void factorizeExponent(int i, int j, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression distributeOnOperandAtIndex(int index, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  void addMissingFactors(Expression factor, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  void factorizeSineAndCosine(int i, int j, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  static bool HaveSameNonNumeralFactors(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalBase(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalExponent(const Expression & e1, const Expression & e2);
  static bool TermHasNumeralBase(const Expression & e);
  static bool TermHasNumeralExponent(const Expression & e);
  static const Expression CreateExponent(Expression e);
  /* Warning: mergeNegativePower doesnot always return  a multiplication:
   *      *(b^-1,c^-1) -> (bc)^-1 */
  Expression mergeNegativePower(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  static inline const Expression Base(const Expression e);
};

}

#endif
