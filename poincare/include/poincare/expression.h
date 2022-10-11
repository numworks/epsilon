#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <poincare/coordinate_2D.h>
#include <poincare/tree_handle.h>
#include <poincare/aliases_list.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression_node.h>
#include <poincare/complex.h>
#include <poincare/solver.h>
#include <ion/storage/file_system.h>
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
  friend class Ceiling;
  friend class Comparison;
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
  friend class DistributionDispatcher;
  friend class Division;
  friend class DivisionQuotient;
  friend class DivisionRemainder;
  friend class Factor;
  friend class Factorial;
  template<typename T>
  friend class FloatList;
  friend class Floor;
  friend class FracPart;
  friend class Function;
  friend class GlobalContext;
  friend class GreatCommonDivisor;
  friend class HyperbolicTrigonometricFunction;
  friend class ImaginaryPart;
  friend class Integer;
  friend class Integral;
  friend class InverseMethod;
  friend class LeastCommonMultiple;
  friend class List;
  friend class ListMean;
  friend class ListMedian;
  friend class ListSampleStandardDeviation;
  friend class ListSequence;
  friend class ListStandardDeviation;
  friend class ListFunctionWithOneOrTwoParametersNode;
  friend class ListVariance;
  friend class Logarithm;
  friend class LogicalOperatorNode;
  friend class Matrix;
  friend class MatrixIdentity;
  friend class MatrixInverse;
  friend class MatrixTrace;
  friend class MatrixTranspose;
  friend class MatrixEchelonForm;
  friend class MatrixRowEchelonForm;
  friend class MatrixReducedRowEchelonForm;
  friend class MixedFraction;
  friend class Multiplication;
  friend class MultiplicationNode;
  friend class NaperianLogarithm;
  friend class NthRoot;
  friend class Number;
  friend class Opposite;
  friend class ParameteredExpression;
  friend class ParameteredExpressionNode;
  friend class Parenthesis;
  friend class PercentAddition;
  friend class PercentSimple;
  friend class PermuteCoefficient;
  friend class PiecewiseOperator;
  friend class Power;
  friend class PowerNode;
  friend class Product;
  friend class Randint;
  friend class RandintNode;
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
  static Expression Parse(char const * string, Context * context, bool addMissingParenthesis = true, bool parseForAssignment = false);
  static Expression ExpressionFromAddress(const void * address, size_t size);

  /* Hierarchy */
  Expression childAtIndex(int i) const;
  void setChildrenInPlace(Expression other) { node()->setChildrenInPlace(other); }

  /* Properties */
  ExpressionNode::Type type() const { return node()->type(); }
  bool isOfType(std::initializer_list<ExpressionNode::Type> types) const { return node()->isOfType(types); }
  TrinaryBoolean isPositive(Context * context) const { return node()->isPositive(context); }
  TrinaryBoolean isNull(Context * context) const { return node()->isNull(context); }
  bool isUndefined() const { return node()->isUndefined(); }
  bool allChildrenAreUndefined();
  bool isNumber() const { return node()->isNumber(); }
  bool isRationalOne() const;
  bool isOne() const;
  bool isMinusOne() const;
  bool isRandom() const { return node()->isRandom(); }
  bool isParameteredExpression() const { return node()->isParameteredExpression(); }
  bool isBasedIntegerCappedBy(const char * integerString) const;
  bool isDivisionOfIntegers() const;
  bool isAlternativeFormOfRationalNumber() const;
  bool hasDefinedComplexApproximation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, float * returnRealPart = nullptr, float * returnImagPart = nullptr) const;
  bool isCombinationOfUnits() const { return node()->isCombinationOfUnits(); }
  /* This two functions only return true if the discontinuity is not asymptotic
   * (i.e. for the functions random, randint, round, floor and ceil).
   * Functions like 1/x are not handled here since it "obvious" that they are
   * discontinuous. */
  bool involvesDiscontinuousFunction(Context * context) const;
  bool isDiscontinuousBetweenValuesForSymbol(const char * symbol, float x1, float x2, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  bool hasBooleanValue() const;

  /* recursivelyMatches will test each node recursively with the function
   * provided as argument. If the result is True, it will stop searching and
   * return true. If the result is Unknown, it will continue searching for a True
   * in the children. If an expression is tested as False, its children will not
   * be tested and the result will be false. */
  typedef TrinaryBoolean (*ExpressionTrinaryTest)(const Expression e, Context * context, void * auxiliary);
  bool recursivelyMatches(ExpressionTrinaryTest test, Context * context = nullptr, ExpressionNode::SymbolicComputation replaceSymbols = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, void * auxiliary = nullptr) const;
  typedef bool (*ExpressionTest)(const Expression e, Context * context);
  bool recursivelyMatches(ExpressionTest test, Context * context = nullptr, ExpressionNode::SymbolicComputation replaceSymbols = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;

  // WARNING: this method must be called on reduced (sorted) expressions
  bool deepIsMatrix(Context * context, bool canContainMatrices = true) const;
  /* Unlike deepIsMatrix, deepIsList can be called on unreduced expressions. */
  bool deepIsList(Context * context) const;
  // Set of ExpressionTest that can be used with recursivelyMatches
  static bool IsNAry(const Expression e, Context * context);
  static bool IsApproximate(const Expression e, Context * context);
  static bool IsRandom(const Expression e, Context * context);
  static bool IsMatrix(const Expression e, Context * context);
  static bool IsInfinity(const Expression e, Context * context);
  static bool IsPercent(const Expression e, Context * context);
  static bool IsDiscontinuous(const Expression e, Context * context);
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
  constexpr static int k_maxNumberOfVariables = 6;
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
  constexpr static int k_maxPolynomialDegree = 3;
  constexpr static int k_maxNumberOfPolynomialCoefficients = k_maxPolynomialDegree+1;
  int getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation) const;
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) { return node()->replaceSymbolWithExpression(symbol, expression); }

  /* Units */
  Expression removeUnit(Expression * unit) { return node()->removeUnit(unit); }
  bool hasUnit() const;

  /* Complex */
  static bool EncounteredComplex();
  static void SetEncounteredComplex(bool encounterComplex);
  bool hasComplexI(Context * context, ExpressionNode::SymbolicComputation replaceSymbols = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  // WARNING: this methods must be called on reduced expressions
  bool isReal(Context * context, bool canContainMatrices = true) const;

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const Expression e) const;
  /* isIdenticalToWithoutParentheses behaves as isIdenticalTo, but without
   * taking into account parentheses: e^(0) is identical to e^0. */
  bool isIdenticalToWithoutParentheses(const Expression e) const;
  bool containsSameDependency(const Expression e, const ExpressionNode::ReductionContext& reductionContext) const;

  static bool ExactAndApproximateBeautifiedExpressionsAreEqual(Expression exactExpression, Expression approximateExpression);

  /* Layout Helper */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context, bool forceStripMargin = false, bool nested = false) const;
  ExpressionNode::LayoutShape leftLayoutShape() const { return node()->leftLayoutShape(); }
  ExpressionNode::LayoutShape rightLayoutShape() const { return node()->rightLayoutShape(); }

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
  Expression cloneAndReduce(ExpressionNode::ReductionContext reductionContext) const;
  // TODO: deepReduceWithSystemCheckpoint should be private but we need to make poincare/text/helper.h a class to be able to friend it
  Expression cloneAndDeepReduceWithSystemCheckpoint(ExpressionNode::ReductionContext * reductionContext, bool * reduceFailure) const;
  // TODO: reduceAndRemoveUnit should be private but we need to make poincare/text/helper.h a class to be able to friend it
  Expression reduceAndRemoveUnit(const ExpressionNode::ReductionContext& reductionContext, Expression * Unit);
  Expression cloneAndReduceOrSimplify(ExpressionNode::ReductionContext reductionContext, bool beautify) { return beautify ? cloneAndSimplify(reductionContext) : cloneAndReduce(reductionContext); }
  /* WARNING: this must be called only on expressions that:
   *  - are reduced.
   *  - have a known sign. (isPositive() != Unknown) */
  Expression setSign(bool positive, const ExpressionNode::ReductionContext& reductionContext);

  Expression deepRemoveUselessDependencies(const ExpressionNode::ReductionContext& reductionContext);

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
  constexpr static int k_noList = -1;
  constexpr static int k_mismatchedLists = -2;
  /* LengthOfListChildren is to be called during reduction, when all children
   * are already reduced. If you expression is NAry, it must be sorted before
   * calling this function. */
  int lengthOfListChildren() const;

  /* Approximation Helper */
  // These methods reset the sApproximationEncounteredComplex flag. They should not be use to implement node approximation
  template<typename U> static U ParseAndSimplifyAndApproximateToScalar(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  template<typename U> Expression approximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce = false) const;
  template<typename U> U approximateToScalar(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce = false) const;
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
    constexpr FunctionHelper(AliasesList aliasesList, const int minNumberOfChildren, const int maxNumberOfChildren, Expression (* const builder)(Expression)) :
      FunctionHelper(aliasesList, minNumberOfChildren, maxNumberOfChildren, builder, nullptr, 0) {}
    constexpr FunctionHelper(AliasesList aliasesList, const int numberOfChildren, Expression (* const builder)(Expression)) :
      FunctionHelper(aliasesList, numberOfChildren, numberOfChildren, builder) {}
    constexpr FunctionHelper(AliasesList aliasesList, const int minNumberOfChildren, const int maxNumberOfChildren, TreeNode::Initializer initializer, size_t size) :
      FunctionHelper(aliasesList, minNumberOfChildren, maxNumberOfChildren, nullptr, initializer, size) {}
    constexpr AliasesList aliasesList() const { return m_aliasesList; }
    int minNumberOfChildren() const { return m_minNumberOfChildren; }
    int maxNumberOfChildren() const { return m_maxNumberOfChildren; }
    int numberOfChildren() const { assert(m_minNumberOfChildren == m_maxNumberOfChildren); return m_minNumberOfChildren; }
    Expression build(Expression children) const;
  private:
    constexpr FunctionHelper(AliasesList aliasesList, const int minNumberOfChildren, const int maxNumberOfChildren, Expression (* const builder)(Expression), TreeNode::Initializer initializer, size_t size) :
      m_aliasesList(aliasesList),
      m_minNumberOfChildren(minNumberOfChildren),
      m_maxNumberOfChildren(maxNumberOfChildren),
      m_untypedBuilder(builder),
      m_initializer(initializer),
      m_size(size)
    {
      assert(minNumberOfChildren >= 0 && minNumberOfChildren <= maxNumberOfChildren);
    }
    AliasesList m_aliasesList;
    const int m_minNumberOfChildren;
    const int m_maxNumberOfChildren;
    Expression (* const m_untypedBuilder)(Expression children);
    TreeNode::Initializer m_initializer;
    const size_t m_size;
  };

  /* Tuple */
  typedef std::initializer_list<Expression> Tuple;

protected:
  Expression(const ExpressionNode * n) : TreeHandle(n) {}

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
    #if ASSERTIONS
      assert(identifier() != TreeNode::NoNodeIdentifier || !TreeHandle::node()->isGhost());
    #endif
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

  /* Simplification */
  static void SimplifyAndApproximateChildren(Expression input, Expression * simplifiedOutput, Expression * approximateOutput, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const ExpressionNode::ReductionContext& reductionContext);
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
  Expression shallowBeautify(const ExpressionNode::ReductionContext& reductionContext) { return node()->shallowBeautify(reductionContext); }
  Expression makePositiveAnyNegativeNumeralFactor(const ExpressionNode::ReductionContext& reductionContext);
  Expression denominator(const ExpressionNode::ReductionContext& reductionContext) const { return node()->denominator(reductionContext); }
  /* shallowReduce takes a copy of reductionContext and not a reference
   * because it might need to modify it during reduction, namely in
   * SimplificationHelper::undefinedOnMatrix */
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext) { return node()->shallowReduce(reductionContext); }
  Expression deepBeautify(const ExpressionNode::ReductionContext& reductionContext) { return node()->deepBeautify(reductionContext); }

  /* Derivation */
  /* This method is used for the reduction of Derivative expressions.
   * It returns whether the instance is differentiable, and differentiates it if
   * able. */
  bool derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue);
  void derivateChildAtIndexInPlace(int index, const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue);
  Expression unaryFunctionDifferential(const ExpressionNode::ReductionContext& reductionContext) { return node()->unaryFunctionDifferential(reductionContext); }

private:
  constexpr static int k_maxSymbolReplacementsCount = 10;
  // Solver parameters
  constexpr static double k_solverStepPrecision = 1e-2;
  constexpr static double k_solverMinimalStep = 1e-3;

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
  constexpr static double k_largestExactIEEE754Integer = 9007199254740992.0;
  Expression deepReduce(const ExpressionNode::ReductionContext& reductionContext);
  void deepReduceChildren(const ExpressionNode::ReductionContext& reductionContext) {
    node()->deepReduceChildren(reductionContext);
  }

  Expression shallowReduceUsingApproximation(const ExpressionNode::ReductionContext& reductionContext);
  Expression defaultShallowBeautify() { return *this; }

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

// Helper to create the expression associated to a node
template<typename T, typename U, int Nmin, int Nmax, typename Parent = Expression>
class ExpressionBuilder : public Parent {
public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<Expression, Parent>::value);
#endif
  ExpressionBuilder(const U * n) : Parent(n) {}
  static T Builder() {
    static_assert(Nmin <= 0 && Nmax >= 0);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child) {
    static_assert(Nmin <= 1 && Nmax >= 1);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child1, Expression child2) {
    static_assert(Nmin <= 2 && Nmax >= 2);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child1, child2});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child1, Expression child2, Expression child3) {
    static_assert(Nmin <= 3 && Nmax >= 3);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child1, child2, child3});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child1, Expression child2, Expression child3, Expression child4) {
    static_assert(Nmin <= 4 && Nmax >= 4);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child1, child2, child3, child4});
    T expression = static_cast<T&>(h);
    return expression;
  }
  constexpr static Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper(U::k_functionName, Nmin, Nmax, Initializer<U>, sizeof(U));
};

template<typename T, typename U, typename P = Expression> using ExpressionNoChildren = ExpressionBuilder<T,U,0,0,P>;
template<typename T, typename U, typename P = Expression> using ExpressionOneChild = ExpressionBuilder<T,U,1,1,P>;
template<typename T, typename U, typename P = Expression> using ExpressionTwoChildren = ExpressionBuilder<T,U,2,2,P>;
template<typename T, typename U, typename P = Expression> using ExpressionUpToTwoChildren = ExpressionBuilder<T,U,1,2,P>;
template<typename T, typename U, typename P = Expression> using ExpressionThreeChildren = ExpressionBuilder<T,U,3,3,P>;
template<typename T, typename U, typename P = Expression> using ExpressionFourChildren = ExpressionBuilder<T,U,4,4,P>;
}

#endif
