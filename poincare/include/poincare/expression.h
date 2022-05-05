#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <poincare/coordinate_2D.h>
#include <poincare/tree_handle.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression_node.h>
#include <poincare/complex.h>
#include <poincare/solver.h>
#include <poincare/simplification_helper.h>
#include <ion/storage.h>
#include <utility>

namespace Poincare {

class Context;
class List;
class Matrix;
class SymbolAbstract;
class Symbol;

class Expression : public TreeHandle {
  friend class AbsoluteValue;
  friend class Addition;
  friend class ArcCosecant;
  friend class ArcCosine;
  friend class ArcCotangent;
  friend class ArcSecant;
  friend class ArcSine;
  friend class ArcTangent;
  friend class Arithmetic;
  friend class BasedInteger;
  friend class BinomialCoefficient;
  friend class BinomialDistributionFunction;
  friend class Ceiling;
  friend class CommonLogarithm;
  friend class ComparisonOperator;
  template<typename T>
  friend class ComplexNode;
  friend class ComplexArgument;
  friend class ComplexCartesian;
  friend class ComplexHelper;
  friend class Conjugate;
  friend class Constant;
  friend class Cosecant;
  friend class Cosine;
  friend class Cotangent;
  friend class Decimal;
  friend class Dependency;
  friend class Derivative;
  friend class Determinant;
  friend class Dimension;
  friend class Division;
  friend class DivisionQuotient;
  friend class DivisionRemainder;
  friend class Equal;
  friend class Factor;
  friend class Factorial;
  friend class Floor;
  friend class FracPart;
  friend class Function;
  friend class GlobalContext;
  friend class GreatCommonDivisor;
  friend class HyperbolicTrigonometricFunction;
  friend class ImaginaryPart;
  friend class Inferior;
  friend class InferiorEqual;
  friend class Integer;
  friend class Integral;
  friend class InvNorm;
  friend class LeastCommonMultiple;
  friend class List;
  friend class ListSequence;
  friend class Logarithm;
  friend class Matrix;
  friend class MatrixIdentity;
  friend class MatrixInverse;
  friend class MatrixTrace;
  friend class MatrixTranspose;
  friend class MatrixEchelonForm;
  friend class MatrixRowEchelonForm;
  friend class MatrixReducedRowEchelonForm;
  friend class Multiplication;
  friend class MultiplicationNode;
  friend class NaperianLogarithm;
  friend class NormalDistributionFunction;
  friend class NormCDF;
  friend class NormCDFRange;
  friend class NormPDF;
  friend class NthRoot;
  friend class Number;
  friend class Opposite;
  friend class ParameteredExpression;
  friend class ParameteredExpressionNode;
  friend class Parenthesis;
  friend class PermuteCoefficient;
  friend class Power;
  friend class PowerNode;
  friend class Product;
  friend class Randint;
  friend class RealPart;
  friend class Round;
  friend class Secant;
  friend class Sequence;
  friend class SequenceNode;
  friend class SignFunction;
  friend class Sine;
  friend class SquareRoot;
  friend class SquareRootNode;
  friend class Store;
  friend class Subtraction;
  friend class SubtractionNode;
  friend class Superior;
  friend class SuperiorEqual;
  friend class Sum;
  friend class SumAndProduct;
  friend class SumAndProductNode;
  friend class Symbol;
  friend class SymbolAbstractNode;
  friend class Tangent;
  friend class Trigonometry;
  friend class TrigonometryCheatTable;
  friend class Unit;
  friend class UnitConvert;
  friend class VectorCross;
  friend class VectorDot;
  friend class VectorNorm;

  friend class AdditionNode;
  friend class DerivativeNode;
  friend class EqualNode;
  template<typename T>
  friend class ExceptionExpressionNode;
  friend class ExpressionNode;
  friend class FunctionNode;
  friend class IntegralNode;
  template<int T>
  friend class LogarithmNode;
  friend class MatrixNode;
  friend class NaperianLogarithmNode;
  friend class NAryExpressionNode;
  friend class NAryInfixExpressionNode;
  friend class StoreNode;
  friend class SymbolNode;
  friend class UnitNode;

  friend class SimplificationHelper;

public:
  static bool IsExpression() { return true; }

  /* Constructor & Destructor */
  Expression() : TreeHandle() {}
  Expression clone() const;
  static Expression Parse(char const * string, Context * context, bool addMissingParenthesis = true);
  static Expression ExpressionFromAddress(const void * address, size_t size);

  /* Hierarchy */
  Expression childAtIndex(int i) const;
  void setChildrenInPlace(Expression other) { node()->setChildrenInPlace(other); }

  /* Properties */
  ExpressionNode::Type type() const { return node()->type(); }
  bool isOfType(ExpressionNode::Type * types, int length) const { return node()->isOfType(types, length); }
  ExpressionNode::Sign sign(Context * context) const { return node()->sign(context); }
  ExpressionNode::NullStatus nullStatus(Context * context) const { return node()->nullStatus(context); }
  bool isStrictly(ExpressionNode::Sign s, Context * context) const { return s == node()->sign(context) && node()->nullStatus(context) == ExpressionNode::NullStatus::NonNull;  }
  bool isUndefined() const { return node()->type() == ExpressionNode::Type::Undefined ||  node()->type() == ExpressionNode::Type::Nonreal; }
  bool isNumber() const { return node()->isNumber(); }
  bool isRationalOne() const;
  bool isRandom() const { return node()->isRandom(); }
  bool isParameteredExpression() const { return node()->isParameteredExpression(); }
  bool isDefinedCosineOrSine(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  bool isBasedIntegerCappedBy(const char * integerString) const;
  bool isDivisionOfIntegers() const;
  bool hasDefinedComplexApproximation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

  /* recursivelyMatches will test each node recursively with the function
   * provided as argument. If the result is Yes, it will stop searching and
   * return true. If the result is Maybe, it will continue searching for a Yes
   * in the children. If an expression is tested as No, its children will not
   * be tested and the result will be false. */
  enum class RecursiveSearchResult {
    Yes,
    Maybe,
    No,
  };
  typedef RecursiveSearchResult (*ExpressionTernaryTest)(const Expression e, Context * context, void * auxiliary);
  bool recursivelyMatches(ExpressionTernaryTest test, Context * context, ExpressionNode::SymbolicComputation replaceSymbols = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, void * auxiliary = nullptr) const;
  typedef bool (*ExpressionTest)(const Expression e, Context * context);
  bool recursivelyMatches(ExpressionTest test, Context * context, ExpressionNode::SymbolicComputation replaceSymbols = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;

  typedef bool (*ExpressionTypeTest)(const Expression e, const void * context);
  // The test should be able to handle uninitialized expressions.
  bool hasExpression(ExpressionTypeTest test, const void * context) const;
  // WARNING: this method must be called on reduced (sorted) expressions
  bool deepIsMatrix(Context * context) const;
  /* Unlike deepIsMatrix, deepIsList can be called on unreduced expressions. */
  bool deepIsList(Context * context) const;
  // Set of ExpressionTest that can be used with recursivelyMatches
  static bool IsNAry(const Expression e, Context * context);
  static bool IsApproximate(const Expression e, Context * context);
  static bool IsRandom(const Expression e, Context * context);
  static bool IsMatrix(const Expression e, Context * context);
  static bool IsInfinity(const Expression e, Context * context);
  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynomial
   * - the degree of the polynomial otherwise */
  int polynomialDegree(Context * context, const char * symbolName) const { return this->node()->polynomialDegree(context, symbolName); }
  /* getVariables fills the matrix variables with the symbols in the expression
   * that pass the test isVariable. It returns the number of entries filled in
   * variables. For instance, getVariables of 'x+y+2*w/cos(4)' would result in
   * variables = {"x", "y", "w"} and would return 3. If the final number of
   * variables would overflow the maxNumberOfVariables, getVariables return -1.
   * If one of the variable lengths overflows maxVariableLength, getVariables
   * returns -2. */
  static constexpr int k_maxNumberOfVariables = 6;
  int getVariables(Context * context, ExpressionNode::isVariableTest isVariable, char * variables, int maxVariableLength, int nextVariableIndex = 0) const { return node()->getVariables(context, isVariable, variables, maxVariableLength, nextVariableIndex); }
  /* getLinearCoefficients return false if the expression is not linear with
   * the variables hold in 'variables'. Otherwise, it fills 'coefficients' with
   * the coefficients of the variables hold in 'variables' (following the same
   * order) and 'constant' with the constant of the expression. */
  bool getLinearCoefficients(char * variables, int maxVariableLength, Expression coefficients[], Expression * constant, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation) const;
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and returns the  polynomial degree.
   * It is supposed to be called on a reduced expression.
   * coefficients has up to 3 entries.  */
  static constexpr int k_maxPolynomialDegree = 3;
  static constexpr int k_maxNumberOfPolynomialCoefficients = k_maxPolynomialDegree+1;
  int getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation) const;
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) { return node()->replaceSymbolWithExpression(symbol, expression); }

  /* Units */
  Expression removeUnit(Expression * unit) { return node()->removeUnit(unit); }
  bool hasUnit() const;

  /* Complex */
  static bool EncounteredComplex();
  static void SetEncounteredComplex(bool encounterComplex);
  static Preferences::ComplexFormat UpdatedComplexFormatWithTextInput(Preferences::ComplexFormat complexFormat, const char * textInput);
  static Preferences::ComplexFormat UpdatedComplexFormatWithExpressionInput(Preferences::ComplexFormat complexFormat, const Expression & e, Context * context);
  bool hasComplexI(Context * context) const;
  // WARNING: this methods must be called on reduced expressions
  bool isReal(Context * context) const;

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const Expression e) const;
  /* isIdenticalToWithoutParentheses behaves as isIdenticalTo, but without
   * taking into account parentheses: e^(0) is identical to e^0. */
  bool isIdenticalToWithoutParentheses(const Expression e) const;
  static bool ParsedExpressionsAreEqual(const char * e0, const char * e1, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat);

  /* Layout Helper */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, bool stripMarginStyle = false, bool nested = false) const;
  /* TODO:
   * - change signature to
   *   size_t serialize(char * buffer, size_t bufferSize...)
   * - Use same convention as strlcpy: return size of the source even if the bufferSize was too small.*/
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const;

  /* Simplification */
  /* Simplification routines are divided in 2 groups:
   * - ParseAndSimplify & simplify methods are used before approximating the
   *   expression. We simplify beforehand to avoid precision error but the
   *   simplified expression is never displayed. The ReductionTarget is
   *   therefore the System for these methods.
   * - ParseAndSimplifyAndApproximate & simplifyAndApproximate methods are used
   *   to simplify and approximate the expression for the User. They take into
   *   account the complex format required in the expression they return.
   *   (For instance, in Polar mode, they return an expression of the form
   *   r*e^(i*th) reduced and approximated.) */
  static Expression ParseAndSimplify(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, ExpressionNode::UnitConversion unitConversion = ExpressionNode::UnitConversion::Default);
  Expression cloneAndSimplify(ExpressionNode::ReductionContext reductionContext);

  static void ParseAndSimplifyAndApproximate(const char * text, Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, ExpressionNode::UnitConversion unitConversion = ExpressionNode::UnitConversion::Default);
  void cloneAndSimplifyAndApproximate(Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, ExpressionNode::UnitConversion unitConversion = ExpressionNode::UnitConversion::Default) const;
  Expression cloneAndReduce(ExpressionNode::ReductionContext context) const;
  // TODO: deepReduceWithSystemCheckpoint should be private but we need to make poincare/text/helper.h a class to be able to friend it
  Expression cloneAndDeepReduceWithSystemCheckpoint(ExpressionNode::ReductionContext * reductionContext, bool * reduceFailure) const;
  // TODO: reduceAndRemoveUnit should be private but we need to make poincare/text/helper.h a class to be able to friend it
  Expression reduceAndRemoveUnit(ExpressionNode::ReductionContext context, Expression * Unit);
  // WARNING: this must be called on reduced expressions
  Expression setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext);

  /* 'ExpressionWithoutSymbols' replaces symbols in place and returns an
   * uninitialized expression if it is circularly defined.
   * SymbolicComputation defines how to handle functions
   * and undefined symbols. */
  static Expression ExpressionWithoutSymbols(Expression expressionWithSymbols, Context * context, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);

  Expression radianToAngleUnit(Preferences::AngleUnit angleUnit);
  Expression angleUnitToRadian(Preferences::AngleUnit angleUnit);

  /* Returns:
   * - a number >= 0 if all the lists have the same size,
   * - -1 if there are no lists in the children
   * - -2 if there are lists of differents lengths. */
  static constexpr int k_noList = -1;
  static constexpr int k_mismatchedLists = -2;
  /* LengthOfListChildren is to be called during reduction, when all children
   * are already reduced. */
  int lengthOfListChildren() const;

  /* Approximation Helper */
  // These methods reset the sApproximationEncounteredComplex flag. They should not be use to implement node approximation
  template<typename U> Expression approximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce = false) const;
  template<typename U> U approximateToScalar(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce = false) const;
  template<typename U> static U ApproximateToScalar(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  template<typename U> U approximateWithValueForSymbol(const char * symbol, U x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  /* Expression roots/extrema solver */
  Coordinate2D<double> nextMinimum(const char * symbol, double start, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, double relativePrecision, double minimalStep, double maximalStep) const;
  Coordinate2D<double> nextMaximum(const char * symbol, double start, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, double relativePrecision, double minimalStep, double maximalStep) const;
  double nextRoot(const char * symbol, double start, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, double relativePrecision, double minimalStep, double maximalStep) const;
  Coordinate2D<double> nextIntersection(const char * symbol, double start, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression, double relativePrecision, double minimalStep, double maximalStep) const;

  /* This class is meant to contain data about named functions (e.g. sin, tan...)
   * in one place: their name, their number of children and a pointer to a builder.
   * It is used in particular by the parser. */
  class FunctionHelper {
  public:
    constexpr FunctionHelper(const char * name, const int numberOfChildren, Expression (* const builder)(Expression)) :
      m_name(name),
      m_numberOfChildren(numberOfChildren),
      m_untypedBuilder(builder) {}
    constexpr const char * name() const { return m_name; }
    int numberOfChildren() const { return m_numberOfChildren; }
    Expression build(Expression children) const { return (*m_untypedBuilder)(children); }
  private:
    const char * m_name;
    const int m_numberOfChildren;
    Expression (* const m_untypedBuilder)(Expression children);
  };

  /* Tuple */
  typedef std::initializer_list<Expression> Tuple;

protected:
  Expression(const ExpressionNode * n) : TreeHandle(n) {}
  Expression(int nodeIdentifier) : TreeHandle(nodeIdentifier) {}
  template<typename U>
  static Expression UntypedBuilderOneChild(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0));
  }
  template<typename U>
  static Expression UntypedBuilderTwoChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1));
  }
  template<typename U>
  static Expression UntypedBuilderThreeChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1), children.childAtIndex(2));
  }
  template<typename U>
  static Expression UntypedBuilderFourChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1), children.childAtIndex(2), children.childAtIndex(3));
  }
  template<typename U>
  static Expression UntypedBuilderMultipleChildren(Expression children) {
    // Only with Expression classes implementing addChildAtIndexInPlace
    assert(children.type() == ExpressionNode::Type::List);
    int childrenNumber = children.numberOfChildren();
    assert(childrenNumber > 0);
    U expression = U::Builder({children.childAtIndex(0)});
    for (int i = 1; i < childrenNumber; ++i) {
      expression.addChildAtIndexInPlace(children.childAtIndex(i), i, i);
    }
    return std::move(expression);
  }

  template<class T> T convert() const {
    /* This function allows to convert Expression to derived Expressions.
     * The asserts ensure that the Expression can only be casted to another
     * Expression, but this does not prevent Expression types mismatches (cast
     * Float to Symbol for instance).
     *
     * We could have overridden the operator T(). However, even with the
     * 'explicit' keyword (which prevents implicit casts), direct initialization
     * are enable which can lead to weird code:
     * ie, you can write: 'Rational a(2); AbsoluteValue b(a);'
     * */

    assert(T::IsExpression());
    static_assert(sizeof(T) == sizeof(Expression), "Size mismatch");
    return *reinterpret_cast<T *>(const_cast<Expression *>(this));
  }

  static_assert(sizeof(TreeHandle::Tuple) == sizeof(Tuple), "Size mismatch");
  static const TreeHandle::Tuple & convert(const Tuple & l) {
    assert(sizeof(TreeHandle) == sizeof(Expression));
    return reinterpret_cast<const TreeHandle::Tuple &>(l);
  }

  /* Reference */
  ExpressionNode * node() const {
    assert(identifier() != TreeNode::NoNodeIdentifier || !TreeHandle::node()->isGhost());
    return static_cast<ExpressionNode *>(TreeHandle::node());
  }

  /* Hierarchy */
  Expression parent() const; // TODO try to inline
  Expression replaceWithUndefinedInPlace();
  void defaultSetChildrenInPlace(Expression other);
  void addChildAtIndexInPlace(TreeHandle t, int index, int currentNumberOfChildren) = delete;
  void removeChildAtIndexInPlace(int i) = delete;
  void removeChildInPlace(TreeHandle t, int childNumberOfChildren) = delete;
  void removeChildrenInPlace(int currentNumberOfChildren) = delete;

  /* Properties */
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const { return node()->getPolynomialCoefficients(context, symbolName, coefficients); }
  Expression defaultReplaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression expression);
  /* 'deepReplaceReplaceableSymbols' replaces symbols in place. isCircular
   * indicates if it is circularly defined and has been interrupted.
   * SymbolicComputation defines how to handle functions and undefined symbols.
   */
  Expression deepReplaceReplaceableSymbols(Context * context, bool * isCircular, int maxSymbolsToReplace, int parameteredAncestorsCount, ExpressionNode::SymbolicComputation symbolicComputation) { return node()->deepReplaceReplaceableSymbols(context, isCircular, maxSymbolsToReplace, parameteredAncestorsCount, symbolicComputation); }
  Expression defaultReplaceReplaceableSymbols(Context * context, bool * isCircular, int maxSymbolsToReplace, int parameteredAncestorsCount, ExpressionNode::SymbolicComputation symbolicComputation);
  Expression defaultOddFunctionSetSign(ExpressionNode::Sign, ExpressionNode::ReductionContext reductionContext);

  /* Simplification */
  static void SimplifyAndApproximateMatrix(Matrix input, Expression * simplifiedOutput, Expression * approximateOutput, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionContext reductionContext);
  static void SimplifyAndApproximateList(List input, Expression * simplifiedOutput, Expression * approximateOutput, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionContext reductionContext);
  void beautifyAndApproximateScalar(Expression * simplifiedExpression, Expression * approximateExpression, ExpressionNode::ReductionContext userReductionContext, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  /* makePositiveAnyNegativeNumeralFactor looks for:
   * - a negative numeral
   * - a multiplication who has one numeral child whose is negative
   * and turns the negative factor into a positive one.
   * The given Expression should already be reduced and the return Expression
   * is reduced (only a numeral factor was potentially made positive, and if it
   *  was -1, it was removed from the multiplication).
   * Warning: this must be called on reduced expressions
   */
  Expression makePositiveAnyNegativeNumeralFactor(ExpressionNode::ReductionContext reductionContext);
  Expression denominator(ExpressionNode::ReductionContext reductionContext) const { return node()->denominator(reductionContext); }
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext) { return node()->shallowReduce(reductionContext); }
  Expression shallowBeautify(ExpressionNode::ReductionContext * reductionContext) { return node()->shallowBeautify(reductionContext); }
  Expression deepBeautify(ExpressionNode::ReductionContext reductionContext);

  /* Derivation */
  /* This method is used for the reduction of Derivative expressions.
   * It returns whether the instance is differentiable, and differentiates it if
   * able. */
  bool derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue);
  void derivateChildAtIndexInPlace(int index, ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue);
  Expression unaryFunctionDifferential(ExpressionNode::ReductionContext reductionContext) { return node()->unaryFunctionDifferential(reductionContext); }

private:
  static constexpr int k_maxSymbolReplacementsCount = 10;
  // Solver parameters
  static constexpr double k_solverStepPrecision = 1e-2;
  static constexpr double k_solverMinimalStep = 1e-3;

  /* Add missing parenthesis will add parentheses that easen the reading of the
   * expression or that are required by math rules. For example:
   * 2+-1 --> 2+(-1)
   * *(+(2,1),3) --> (2+1)*3
   */
  Expression addMissingParentheses();
  void shallowAddMissingParenthesis();

  /* Simplification */
  /* The largest integer such that all smaller integers can be stored without
   * any precision loss in IEEE754 double representation is 2E53 as the
   * mantissa is stored on 53 bits (2E308 can be stored exactly in IEEE754
   * representation but some smaller integers can't - like 2E308-1). */
  static constexpr double k_largestExactIEEE754Integer = 9007199254740992.0;
  Expression deepReduce(ExpressionNode::ReductionContext reductionContext);
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
    node()->deepReduceChildren(reductionContext);
  }

  Expression shallowReduceUsingApproximation(ExpressionNode::ReductionContext reductionContext);
  Expression defaultShallowBeautify() { return *this; }
  void deepBeautifyChildren(ExpressionNode::ReductionContext reductionContext) {
    node()->deepBeautifyChildren(reductionContext);
  }

  bool defaultDidDerivate() { return false; }
  Expression defaultUnaryFunctionDifferential() { return *this; }

  /* Approximation */
  template<typename U> Evaluation<U> approximateToEvaluation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce = false) const;

  /* Properties */
  int defaultGetPolynomialCoefficients(Context * context, const char * symbol, Expression expression[]) const;

  /* Builder */
  static bool IsZero(const Expression e);
  static bool IsOne(const Expression e);
  static bool IsMinusOne(const Expression e);
  static Expression CreateComplexExpression(Expression ra, Expression tb, Preferences::ComplexFormat complexFormat, bool undefined, bool isZeroRa, bool isOneRa, bool isZeroTb, bool isOneTb, bool isNegativeRa, bool isNegativeTb);
};

}

#endif
