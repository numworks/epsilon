#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/approximation_helper.h>
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
  Sign sign() const override;
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;

  // Approximation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d) { return Complex<T>(c*d); }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n);

private:
  // Property
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;

  // Layout
  bool childNeedsParenthesis(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override;

  /* Approximation */
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

class Multiplication final : public NAryExpression {
  friend class AdditionNode;
  friend class Addition;
  friend class Power;
public:
  Multiplication(const MultiplicationNode * n) : NAryExpression(n) {}
  Multiplication();
  explicit Multiplication(Expression e1) : Multiplication() {
    addChildAtIndexInPlace(e1, 0, 0);
  }
  Multiplication(Expression e1, Expression e2) : Multiplication() {
    addChildAtIndexInPlace(e2, 0, 0);
    addChildAtIndexInPlace(e1, 0, numberOfChildren());
  }
  Multiplication(Expression e1, Expression e2, Expression e3) : Multiplication(e2, e3) {
    addChildAtIndexInPlace(e1, 0, numberOfChildren());
  }

  template<typename T> static void computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);
  // Expression
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit);
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const;
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const;
private:
  // Simplification
  Expression privateShallowReduce(Context& context, Preferences::AngleUnit angleUnit, bool expand, bool canBeInterrupted);
  void mergeMultiplicationChildrenInPlace();
  void factorizeBase(int i, int j, Context & context, Preferences::AngleUnit angleUnit);
  void mergeInChildByFactorizingBase(int i, Expression e, Context & context, Preferences::AngleUnit angleUnit);
  void factorizeExponent(int i, int j, Context & context, Preferences::AngleUnit angleUnit);
  Expression distributeOnOperandAtIndex(int index, Context & context, Preferences::AngleUnit angleUnit);
  void addMissingFactors(Expression factor, Context & context, Preferences::AngleUnit angleUnit);
  void factorizeSineAndCosine(int i, int j, Context & context, Preferences::AngleUnit angleUnit);
  static bool HaveSameNonNumeralFactors(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalBase(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalExponent(const Expression & e1, const Expression & e2);
  static bool TermHasNumeralBase(const Expression & e);
  static bool TermHasNumeralExponent(const Expression & e);
  static const Expression CreateExponent(Expression e);
  /* Warning: mergeNegativePower doesnot always return  a multiplication:
   *      *(b^-1,c^-1) -> (bc)^-1 */
  Expression mergeNegativePower(Context & context, Preferences::AngleUnit angleUnit);
  static inline const Expression Base(const Expression e);
};

}

#endif
