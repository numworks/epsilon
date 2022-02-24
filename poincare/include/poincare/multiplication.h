#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/approximation_helper.h>
#include <poincare/n_ary_infix_expression.h>

namespace Poincare {

class MultiplicationNode final : public NAryInfixExpressionNode {
  friend class Addition;
public:
  using NAryInfixExpressionNode::NAryInfixExpressionNode;

  // Tree
  size_t size() const override { return sizeof(MultiplicationNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Multiplication";
  }
#endif

  // Properties
  Type type() const override { return Type::Multiplication; }
  virtual IntegerStatus integerStatus(Context * context) const override;
  Sign sign(Context * context) const override;
  int polynomialDegree(Context * context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const override;
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;
  Expression removeUnit(Expression * unit) override;

  // Approximation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) { return Complex<T>::Builder(c*d); }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);

private:
  // Property
  Expression setSign(Sign s, ReductionContext reductionContext) override;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  CodePoint operatorSymbol() const;

  // Serialize
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext * reductionContext) override;
  Expression denominator(ExpressionNode::ReductionContext reductionContext) const override;
  // Derivation
  bool derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) override;

  // Approximation
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapReduce<float>(this, approximationContext, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapReduce<double>(this, approximationContext, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

class Multiplication : public NAryExpression {
  friend class Addition;
  friend class Power;
  friend class MultiplicationNode;
public:
  Multiplication(const MultiplicationNode * n) : NAryExpression(n) {}
  static Multiplication Builder(const Tuple & children = {}) { return TreeHandle::NAryBuilder<Multiplication, MultiplicationNode>(convert(children)); }
  // TODO: Get rid of those helpers
  static Multiplication Builder(Expression e1) { return Multiplication::Builder({e1}); }
  static Multiplication Builder(Expression e1, Expression e2) { return Multiplication::Builder({e1, e2}); }
  static Multiplication Builder(Expression e1, Expression e2, Expression e3) { return Multiplication::Builder({e1, e2, e3}); }
  static Multiplication Builder(Expression e1, Expression e2, Expression e3, Expression e4) { return Multiplication::Builder({e1, e2, e3, e4}); }

  // Properties
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const;

  // Approximation
  template<typename T> static void computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);
  // Simplification
  Expression setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext * reductionContext);
  Expression denominator(ExpressionNode::ReductionContext reductionContext) const;
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order, Context * context, bool canBeInterrupted) {
    NAryExpression::sortChildrenInPlace(order, context, false, canBeInterrupted);
  }
  // Derivation
  bool derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue);
private:
  // Unit
  Expression removeUnit(Expression * unit);

  // Simplification
  Expression privateShallowReduce(ExpressionNode::ReductionContext reductionContext, bool expand, bool canBeInterrupted);
  void factorizeBase(int i, int j, ExpressionNode::ReductionContext reductionContext);
  void mergeInChildByFactorizingBase(int i, Expression e, ExpressionNode::ReductionContext reductionContext);
  void factorizeExponent(int i, int j, ExpressionNode::ReductionContext reductionContext);
  Expression distributeOnOperandAtIndex(int index, ExpressionNode::ReductionContext reductionContext);
  void addMissingFactors(Expression factor, ExpressionNode::ReductionContext reductionContext);
  void factorizeSineAndCosine(int i, int j, ExpressionNode::ReductionContext reductionContext);
  static bool HaveSameNonNumeralFactors(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalBase(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalExponent(const Expression & e1, const Expression & e2);
  static bool TermsCanSafelyCombineExponents(const Expression & e1, const Expression & e2, ExpressionNode::ReductionContext reductionContext);
  static bool TermHasNumeralBase(const Expression & e);
  static bool TermHasNumeralExponent(const Expression & e);
  static const Expression CreateExponent(Expression e);
  static inline const Expression Base(const Expression e);
  void splitIntoNormalForm(Expression & numerator, Expression & denominator, ExpressionNode::ReductionContext reductionContext) const;
};

}

#endif
