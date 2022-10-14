#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/approximation_helper.h>
#include <poincare/n_ary_infix_expression.h>

namespace Poincare {

class MultiplicationNode final : public NAryInfixExpressionNode {
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
  TrinaryBoolean isPositive(Context * context) const override;
  TrinaryBoolean isNull(Context * context) const override;
  int polynomialDegree(Context * context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const override;
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;
  Expression removeUnit(Expression * unit) override;
  double degreeForSortingAddition(bool symbolsOnly) const override;

  // Approximation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixAndComplex(m, c, complexFormat, computeOnComplex<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  template<typename T> static Evaluation<T> Compute(Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::Reduce<T>(
        eval1,
        eval2,
        complexFormat,
        computeOnComplex<T>,
        computeOnComplexAndMatrix<T>,
        computeOnMatrixAndComplex<T>,
        computeOnMatrices<T>);
  }

private:
  enum class MultiplicationSymbol : uint8_t {
  // The order matters !
    Empty = 0,
    MiddleDot = 1,
    MultiplicationSign = 2,
  };

  static MultiplicationSymbol OperatorSymbolBetween(ExpressionNode::LayoutShape left, ExpressionNode::LayoutShape right);
  static CodePoint CodePointForOperatorSymbol(MultiplicationSymbol symbol);
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  CodePoint operatorSymbol() const;

  // Serialize
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) override;

  // Approximation
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixAndComplex(m, c, complexFormat, computeOnComplex<T>);
  }
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapReduce<float>(this, approximationContext, Compute<float>);
   }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapReduce<double>(this, approximationContext, Compute<double>);
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
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const;

  // Approximation
  template<typename T> static void computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);
  // Simplification
  Expression shallowBeautify(const ExpressionNode::ReductionContext& reductionContext);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression denominator(const ExpressionNode::ReductionContext& reductionContext) const;
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order, Context * context, bool canContainMatrices = true) {
    NAryExpression::sortChildrenInPlace(order, context, false, canContainMatrices);
  }
  // Derivation
  bool derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue);
private:
  // Unit
  Expression removeUnit(Expression * unit);

  // Simplification
  void factorizeBase(int i, int j, const ExpressionNode::ReductionContext& reductionContext, List dependenciesCreatedDuringReduction);
  void mergeInChildByFactorizingBase(int i, Expression e, const ExpressionNode::ReductionContext& reductionContext, List dependenciesCreatedDuringReduction = List());
  void factorizeExponent(int i, int j, const ExpressionNode::ReductionContext& reductionContext);
  Expression gatherLikeTerms(const ExpressionNode::ReductionContext & reductionContext);
  bool gatherRationalPowers(int i, int j, const ExpressionNode::ReductionContext& reductionContext);
  Expression distributeOnOperandAtIndex(int index, const ExpressionNode::ReductionContext& reductionContext);
  // factor must be a reduced expression
  void addMissingFactors(Expression factor, const ExpressionNode::ReductionContext& reductionContext);
  bool factorizeSineAndCosine(int i, int j, const ExpressionNode::ReductionContext& reductionContext);
  static bool HaveSameNonNumeralFactors(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalBase(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalExponent(const Expression & e1, const Expression & e2);
  static bool TermHasNumeralBase(const Expression & e);
  static bool TermHasNumeralExponent(const Expression & e);
  static bool TermIsPowerOfRationals(const Expression & e);
  static const Expression CreateExponent(Expression e);
  static inline const Expression Base(const Expression e);
  void splitIntoNormalForm(Expression & numerator, Expression & denominator, const ExpressionNode::ReductionContext& reductionContext) const;
};

}

#endif
