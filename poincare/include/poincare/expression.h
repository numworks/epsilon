#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <poincare/array_builder.h>
#include <poincare/coordinate_2D.h>
#include <poincare/tree_handle.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression_node.h>
#include <poincare/complex.h>
#include <poincare/solver.h>
#include <ion/storage.h>

#include <stdio.h>

namespace Poincare {

class Context;
class SymbolAbstract;
class Symbol;

class Expression : public TreeHandle {
  friend class AbsoluteValue;
  friend class Addition;
  friend class ArcCosine;
  friend class ArcSine;
  friend class ArcTangent;
  friend class Arithmetic;
  friend class BasedInteger;
  friend class BinomialCoefficient;
  friend class BinomialDistributionFunction;
  friend class Ceiling;
  friend class CommonLogarithm;
  template<typename T>
  friend class ComplexNode;
  friend class ComplexArgument;
  friend class ComplexCartesian;
  friend class ComplexHelper;
  friend class ConfidenceInterval;
  friend class Conjugate;
  friend class Cosine;
  friend class Decimal;
  friend class Derivative;
  friend class Determinant;
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
  friend class Integer;
  friend class Integral;
  friend class InvNorm;
  friend class LeastCommonMultiple;
  friend class Logarithm;
  friend class Matrix;
  friend class MatrixDimension;
  friend class MatrixIdentity;
  friend class MatrixInverse;
  friend class MatrixTrace;
  friend class MatrixTranspose;
  friend class Multiplication;
  friend class MultiplicationNode;
  friend class NaperianLogarithm;
  friend class NormalDistributionFunction;
  friend class NormCDF;
  friend class NormCDF2;
  friend class NormPDF;
  friend class NthRoot;
  friend class Number;
  friend class Opposite;
  friend class ParameteredExpression;
  friend class Parenthesis;
  friend class PermuteCoefficient;
  friend class Power;
  friend class PowerNode;
  friend class PredictionInterval;
  friend class Product;
  friend class Randint;
  friend class RealPart;
  friend class Round;
  friend class Sequence;
  friend class SignFunction;
  friend class Sine;
  friend class SquareRoot;
  friend class SquareRootNode;
  friend class Store;
  friend class Subtraction;
  friend class SubtractionNode;
  friend class Sum;
  friend class Symbol;
  friend class SymbolAbstractNode;
  friend class Tangent;
  friend class Trigonometry;
  friend class TrigonometryCheatTable;
  friend class Unit;
  friend class UnitConvert;

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
  friend class StoreNode;
  friend class SymbolNode;

public:
  static bool IsExpression() { return true; }

  /* Constructor & Destructor */
  Expression() : TreeHandle() {}
  Expression clone() const;
  static Expression Parse(char const * string, Context * context, bool addMissingParenthesis = true);
  static Expression ExpressionFromAddress(const void * address, size_t size);

  /* Circuit breaker */
  typedef bool (*CircuitBreaker)();
  static void SetCircuitBreaker(CircuitBreaker cb);
  static bool ShouldStopProcessing();
  static void SetInterruption(bool interrupt);

  /* Hierarchy */
  Expression childAtIndex(int i) const;
  void setChildrenInPlace(Expression other) { node()->setChildrenInPlace(other); }

  /* Properties */
  ExpressionNode::Type type() const { return node()->type(); }
  bool isOfType(ExpressionNode::Type * types, int length) const { return node()->isOfType(types, length); }
  ExpressionNode::Sign sign(Context * context) const { return node()->sign(context); }
  bool isUndefined() const { return node()->type() == ExpressionNode::Type::Undefined ||  node()->type() == ExpressionNode::Type::Unreal; }
  bool isNumber() const { return node()->isNumber(); }
  bool isRationalZero() const;
  bool isRationalOne() const;
  bool isRandom() const { return node()->isRandom(); }
  bool isParameteredExpression() const { return node()->isParameteredExpression(); }
  bool isDefinedCosineOrSine(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  bool isBasedIntegerCappedBy(const char * integerString) const;
  bool isDivisionOfIntegers() const;
  bool hasDefinedComplexApproximation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  typedef bool (*ExpressionTest)(const Expression e, Context * context);
  bool recursivelyMatches(ExpressionTest test, Context * context, bool replaceSymbols = true) const;
  typedef bool (*ExpressionTypeTest)(const Expression e, const void * context);
  bool hasExpression(ExpressionTypeTest test, const void * context) const;
  // WARNING: this method must be called on reduced (sorted) expressions
  bool deepIsMatrix(Context * context) const;
  // Set of ExpressionTest that can be used with recursivelyMatches
  static bool IsNAry(const Expression e, Context * context);
  static bool IsApproximate(const Expression e, Context * context);
  static bool IsRandom(const Expression e, Context * context);
  static bool IsMatrix(const Expression e, Context * context);
  static bool IsInfinity(const Expression e, Context * context);
  /* 'characteristicXRange' tries to assess the range on x where the expression
   * (considered as a function on x) has an interesting evolution. For example,
   * the period of the function on 'x' if it is periodic. If
   * the function is x-independent, the return value is 0.0f (because any
   * x-range is equivalent). If the function does not have an interesting range,
   * the return value is NAN.
   * NB: so far, we consider that the only way of building a periodic function
   * is to use sin/tan/cos(f(x)) with f a linear function. */
  float characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const { return node()->characteristicXRange(context, angleUnit); }
  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynome
   * - the degree of the polynome otherwise */
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
  bool getLinearCoefficients(char * variables, int maxVariableLength, Expression coefficients[], Expression constant[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::SymbolicComputation symbolicComputation) const;
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and returns the  polynomial degree.
   * It is supposed to be called on a reduced expression.
   * coefficients has up to 3 entries.  */
  static constexpr int k_maxPolynomialDegree = 2;
  static constexpr int k_maxNumberOfPolynomialCoefficients = k_maxPolynomialDegree+1;
  int getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::SymbolicComputation symbolicComputation) const;
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) { return node()->replaceSymbolWithExpression(symbol, expression); }

  /* Units */
  Expression getUnit() const { return node()->getUnit(); }
  bool hasUnit() const;

  /* Complex */
  static bool EncounteredComplex();
  static void SetEncounteredComplex(bool encounterComplex);
  static Preferences::ComplexFormat UpdatedComplexFormatWithTextInput(Preferences::ComplexFormat complexFormat, const char * textInput);
  static Preferences::ComplexFormat UpdatedComplexFormatWithExpressionInput(Preferences::ComplexFormat complexFormat, const Expression & e, Context * context);
  // WARNING: this methods must be called on reduced expressions
  bool isReal(Context * context) const;

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const Expression e) const;
  static bool ParsedExpressionsAreEqual(const char * e0, const char * e1, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);

  /* Layout Helper */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
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
  static Expression ParseAndSimplify(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  Expression simplify(ExpressionNode::ReductionContext reductionContext);

  static void ParseAndSimplifyAndApproximate(const char * text, Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  void simplifyAndApproximate(Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  Expression reduce(ExpressionNode::ReductionContext context);

  Expression mapOnMatrixFirstChild(ExpressionNode::ReductionContext reductionContext);
  /* 'ExpressionWithoutSymbols' returns an uninitialized expression if it is
   * circularly defined. Same convention as for 'deepReplaceReplaceableSymbols'.*/
  static Expression ExpressionWithoutSymbols(Expression expressionWithSymbols, Context * context, bool replaceFunctionsOnly = false);

  Expression radianToAngleUnit(Preferences::AngleUnit angleUnit);
  Expression angleUnitToRadian(Preferences::AngleUnit angleUnit);

  /* Approximation Helper */
  // These methods reset the sApproximationEncounteredComplex flag. They should not be use to implement node approximation
  template<typename U> static U Epsilon();
  template<typename U> Expression approximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename U> U approximateToScalar(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename U> static U ApproximateToScalar(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::SymbolicComputation symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  template<typename U> U approximateWithValueForSymbol(const char * symbol, U x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  /* Expression roots/extrema solver */
  Coordinate2D<double> nextMinimum(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  Coordinate2D<double> nextMaximum(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  double nextRoot(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  Coordinate2D<double> nextIntersection(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const;

  /* This class is meant to contain data about named functions (e.g. sin, tan...)
   * in one place: their name, their number of children and a pointer to a builder.
   * It is used in particular by the parser. */
  class FunctionHelper {
  public:
    constexpr FunctionHelper(const char * name, const int numberOfChildren, Expression (* const builder)(Expression)) :
      m_name(name),
      m_numberOfChildren(numberOfChildren),
      m_untypedBuilder(builder) {}
    const char * name() const { return m_name; }
    int numberOfChildren() const { return m_numberOfChildren; }
    Expression build(Expression children) const { return (*m_untypedBuilder)(children); }
  private:
    const char * m_name;
    const int m_numberOfChildren;
    Expression (* const m_untypedBuilder)(Expression children);
  };

  static void Tidy() { sSymbolReplacementsCountLock = false; }

protected:
  static bool SimplificationHasBeenInterrupted();
  Expression(const ExpressionNode * n) : TreeHandle(n) {}
  Expression(int nodeIdentifier) : TreeHandle(nodeIdentifier) {}
  template<typename U>
  static Expression UntypedBuilderOneChild(Expression children) {
    assert(children.type() == ExpressionNode::Type::Matrix);
    return U::Builder(children.childAtIndex(0));
  }
  template<typename U>
  static Expression UntypedBuilderTwoChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::Matrix);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1));
  }
  template<typename U>
  static Expression UntypedBuilderThreeChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::Matrix);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1), children.childAtIndex(2));
  }
  template<typename U>
  static Expression UntypedBuilderFourChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::Matrix);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1), children.childAtIndex(2), children.childAtIndex(3));
  }

  template<class T> T convert() const {
    /* This function allows to convert Expression to derived Expressions.
     * The asserts ensure that the Expression can only be casted to another
     * Expression, but this does not prevent Expression types mismatches (cast
     * Float to Symbol for instance).
     *
     * We could have overriden the operator T(). However, even with the
     * 'explicit' keyword (which prevents implicit casts), direct initilization
     * are enable which can lead to weird code:
     * ie, you can write: 'Rational a(2); AbsoluteValue b(a);'
     * */

    assert(T::IsExpression());
    static_assert(sizeof(T) == sizeof(Expression), "Size mismatch");
    return *reinterpret_cast<T *>(const_cast<Expression *>(this));
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
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const { return node()->getPolynomialCoefficients(context, symbolName, coefficients, symbolicComputation); }
  Expression defaultReplaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression expression);
  /* 'deepReplaceReplaceableSymbols' returns an uninitialized expression if it
   * is circularly defined. Same convention as for 'ExpressionWithoutSymbols'.*/
  Expression deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) { return node()->deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount); }
  Expression defaultReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount);

  /* Simplification */
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
  Expression shallowBeautify(ExpressionNode::ReductionContext reductionContext) { return node()->shallowBeautify(reductionContext); }
  Expression deepBeautify(ExpressionNode::ReductionContext reductionContext);
  // WARNING: this must be called on reduced expressions
  Expression setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext);

private:
  static constexpr int k_maxSymbolReplacementsCount = 10;
  static bool sSymbolReplacementsCountLock;

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
    return node()->deepReduceChildren(reductionContext);
  }
  void defaultDeepReduceChildren(ExpressionNode::ReductionContext reductionContext);
  Expression defaultShallowReduce();
  Expression defaultHandleUnitsInChildren(); // Children must be reduced
  Expression shallowReduceUsingApproximation(ExpressionNode::ReductionContext reductionContext);
  Expression defaultShallowBeautify() { return *this; }

  /* Approximation */
  template<typename U> Evaluation<U> approximateToEvaluation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

  /* Properties */
  int defaultGetPolynomialCoefficients(Context * context, const char * symbol, Expression expression[]) const;

  /* Builder */
  static bool IsZero(const Expression e);
  static bool IsOne(const Expression e);
  static bool IsMinusOne(const Expression e);
  static Expression CreateComplexExpression(Expression ra, Expression tb, Preferences::ComplexFormat complexFormat, bool undefined, bool isZeroRa, bool isOneRa, bool isZeroTb, bool isOneTb, bool isNegativeRa, bool isNegativeTb);

  /* Expression roots/extrema solver*/
  constexpr static double k_solverPrecision = 1.0E-5;
  constexpr static double k_maxFloat = 1e100;
  Coordinate2D<double> nextMinimumOfExpression(const char * symbol, double start, double step, double max, Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression = Expression(), bool lookForRootMinimum = false) const;
  void bracketMinimum(const char * symbol, double start, double step, double max, double result[3], Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression = Expression()) const;
  double nextIntersectionWithExpression(const char * symbol, double start, double step, double max, Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const;
  void bracketRoot(const char * symbol, double start, double step, double max, double result[2], Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const;
};

}

#endif
