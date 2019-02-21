#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <poincare/tree_handle.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression_node.h>
#include <poincare/complex.h>
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
  friend class BinomialCoefficient;
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
  friend class Integral;
  friend class LeastCommonMultiple;
  friend class Logarithm;
  friend class Matrix;
  friend class MatrixDimension;
  friend class MatrixInverse;
  friend class MatrixTrace;
  friend class MatrixTranspose;
  friend class Multiplication;
  friend class NaperianLogarithm;
  friend class NthRoot;
  friend class Number;
  friend class Opposite;
  friend class Parenthesis;
  friend class PermuteCoefficient;
  friend class Power;
  friend class PowerNode;
  friend class PredictionInterval;
  friend class Product;
  friend class RealPart;
  friend class Round;
  friend class SignFunction;
  friend class Sine;
  friend class SquareRoot;
  friend class SquareRootNode;
  friend class Store;
  friend class Subtraction;
  friend class Sum;
  friend class Symbol;
  friend class SymbolAbstractNode;
  friend class Tangent;
  friend class Trigonometry;
  friend class TrigonometryCheatTable;

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
  friend class NaperianLogarithmNode;
  friend class NAryExpressionNode;
  friend class StoreNode;
  friend class SymbolNode;

public:
  static bool IsExpression() { return true; }

  /* Constructor & Destructor */
  Expression() : TreeHandle() {}
  Expression clone() const;
  static Expression Parse(char const * string);
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
  ExpressionNode::Sign sign(Context * context) const { return node()->sign(context); }
  bool isUndefined() const { return node()->type() == ExpressionNode::Type::Undefined ||  node()->type() == ExpressionNode::Type::Unreal; }
  bool isNumber() const { return node()->isNumber(); }
  bool isRationalZero() const;
  bool isRationalOne() const;
  typedef bool (*ExpressionTest)(const Expression e, Context & context, bool replaceSymbols);
  bool recursivelyMatches(ExpressionTest test, Context & context, bool replaceSymbols) const;
  bool isApproximate(Context & context) const;
  bool recursivelyMatchesInfinity(Context & context) { return recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) { return e.type() == ExpressionNode::Type::Infinity; }, context, true); }
  static bool IsMatrix(const Expression e, Context & context, bool replaceSymbols);
  /* 'characteristicXRange' tries to assess the range on x where the expression
   * (considered as a function on x) has an interesting evolution. For example,
   * the period of the function on 'x' if it is periodic. If
   * the function is x-independent, the return value is 0.0f (because any
   * x-range is equivalent). If the function does not have an interesting range,
   * the return value is NAN.
   * NB: so far, we consider that the only way of building a periodic function
   * is to use sin/tan/cos(f(x)) with f a linear function. */
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const { return node()->characteristicXRange(context, angleUnit); }
  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynome
   * - the degree of the polynome otherwise */
  int polynomialDegree(Context & context, const char * symbolName) const { return this->node()->polynomialDegree(context, symbolName); }
  /* getVariables fills the matrix variables with the symbols in the expression
   * that pass the test isVariable. It returns the number of entries filled in
   * variables. For instance, getVariables of 'x+y+2*w/cos(4)' would result in
   * variables = {"x", "y", "w"} and would return 3. If the final number of
   * variables would overflow the maxNumberOfVariables, getVariables return -1.
   * If one of the variable lengths overflows maxVariableLength, getVariables
   * returns -2. */
  static constexpr int k_maxNumberOfVariables = 6;
  int getVariables(Context & context, ExpressionNode::isVariableTest isVariable, char * variables, int maxVariableLength) const { return node()->getVariables(context, isVariable, variables, maxVariableLength); }
  /* getLinearCoefficients return false if the expression is not linear with
   * the variables hold in 'variables'. Otherwise, it fills 'coefficients' with
   * the coefficients of the variables hold in 'variables' (following the same
   * order) and 'constant' with the constant of the expression. */
  bool getLinearCoefficients(char * variables, int maxVariableLength, Expression coefficients[], Expression constant[], Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and returns the  polynomial degree.
   * It is supposed to be called on a reduced expression.
   * coefficients has up to 3 entries.  */
  static constexpr int k_maxPolynomialDegree = 2;
  static constexpr int k_maxNumberOfPolynomialCoefficients = k_maxPolynomialDegree+1;
  int getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) { return node()->replaceSymbolWithExpression(symbol, expression); }
  Expression replaceUnknown(const Symbol & symbol);
  Expression defaultReplaceUnknown(const Symbol & symbol);

  /* Complex */
  static bool EncounteredComplex();
  static void SetEncounteredComplex(bool encounterComplex);
  static Preferences::ComplexFormat UpdatedComplexFormatWithTextInput(Preferences::ComplexFormat complexFormat, const char * textInput);
  static Preferences::ComplexFormat UpdatedComplexFormatWithExpressionInput(Preferences::ComplexFormat complexFormat, const Expression & e, Context & context);
  bool isReal(Context & context) const { return node()->isReal(context); }

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const Expression e) const;
  bool isEqualToItsApproximationLayout(Expression approximation, char * buffer, int bufferSize, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context & context);

  /* Layout Helper */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
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
  static Expression ParseAndSimplify(const char * text, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Expression simplify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);

  static void ParseAndSimplifyAndApproximate(const char * text, Expression * simplifiedExpression, Expression * approximateExpression, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  void simplifyAndApproximate(Expression * simplifiedExpression, Expression * approximateExpression, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Expression reduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);

  static Expression ExpressionWithoutSymbols(Expression expressionWithSymbols, Context & context);
  Expression radianToDegree();
  Expression degreeToRadian();

  /* Approximation Helper */
  // These methods reset the sApproximationEncounteredComplex flag. They should not be use to implement node approximation
  template<typename U> static U Epsilon();
  template<typename U> Expression approximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename U> U approximateToScalar(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename U> static U ApproximateToScalar(const char * text, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  template<typename U> U approximateWithValueForSymbol(const char * symbol, U x, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  /* Expression roots/extrema solver */
  struct Coordinate2D {
    double abscissa;
    double value;
  };
  Coordinate2D nextMinimum(const char * symbol, double start, double step, double max, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  Coordinate2D nextMaximum(const char * symbol, double start, double step, double max, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  double nextRoot(const char * symbol, double start, double step, double max, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  Coordinate2D nextIntersection(const char * symbol, double start, double step, double max, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const;

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
    const int numberOfChildren() const { return m_numberOfChildren; }
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
  Expression(int nodeIdentifier) : TreeHandle(nodeIdentifier) {}
  Expression parent() const; // TODO try to inline
  void defaultSetChildrenInPlace(Expression other);
  void addChildAtIndexInPlace(TreeHandle t, int index, int currentNumberOfChildren) = delete;
  void removeChildAtIndexInPlace(int i) = delete;
  void removeChildInPlace(TreeHandle t, int childNumberOfChildren) = delete;
  void removeChildrenInPlace(int currentNumberOfChildren) = delete;

  /* Properties */
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const { return node()->getPolynomialCoefficients(context, symbolName, coefficients); }
  bool hasReplaceableSymbols(Context & context) const;
  Expression shallowReplaceReplaceableSymbols(Context & context) { return node()->shallowReplaceReplaceableSymbols(context); }
  Expression defaultReplaceReplaceableSymbols(Context & context);

  /* Simplification */
  /* makePositiveAnyNegativeNumeralFactor looks for:
   * - a negative numeral
   * - a multiplication who has one numeral child whose is negative
   * and turns the negative factor into a positive one.
   * The given Expression should already be reduced and the return Expression
   * is reduced (only a numeral factor was potentially made positive, and if it
   *  was -1, it was removed from the multiplication).
   */
  Expression makePositiveAnyNegativeNumeralFactor(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const { return node()->denominator(context, complexFormat, angleUnit); }
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) { return node()->shallowReduce(context, complexFormat, angleUnit, target); }
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) { return node()->shallowBeautify(context, complexFormat, angleUnit, target); }
  Expression deepBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);

private:
  static constexpr int k_maxSymbolReplacementsCount = 10;
  static bool sSymbolReplacementsCountLock;
  /* Simplification */
  Expression deepReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  void deepReduceChildren(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
    return node()->deepReduceChildren(context, complexFormat, angleUnit, target);
  }
  void defaultDeepReduceChildren(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression defaultShallowReduce();
  Expression defaultShallowBeautify() { return *this; }

  /* Approximation */
  template<typename U> Evaluation<U> approximateToEvaluation(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

  /* Properties */
  Expression defaultReplaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression expression);
  int defaultGetPolynomialCoefficients(Context & context, const char * symbol, Expression expression[]) const;

  /* Builder */
  static bool IsZero(const Expression e);
  static bool IsOne(const Expression e);
  static bool IsMinusOne(const Expression e);
  static Expression CreateComplexExpression(Expression ra, Expression tb, Preferences::ComplexFormat complexFormat, bool undefined, bool isZeroRa, bool isOneRa, bool isZeroTb, bool isOneTb, bool isNegativeRa, bool isNegativeTb);

  /* Expression roots/extrema solver*/
  constexpr static double k_solverPrecision = 1.0E-5;
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
  constexpr static double k_maxFloat = 1e100;
  typedef double (*EvaluationAtAbscissa)(const char * symbol, double abscissa, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression0, const Expression expression1);
  Coordinate2D nextMinimumOfExpression(const char * symbol, double start, double step, double max, EvaluationAtAbscissa evaluation, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression = Expression(), bool lookForRootMinimum = false) const;
  void bracketMinimum(const char * symbol, double start, double step, double max, double result[3], EvaluationAtAbscissa evaluation, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression = Expression()) const;
  Coordinate2D brentMinimum(const char * symbol, double ax, double bx, EvaluationAtAbscissa evaluation, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression = Expression()) const;
  double nextIntersectionWithExpression(const char * symbol, double start, double step, double max, EvaluationAtAbscissa evaluation, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const;
  void bracketRoot(const char * symbol, double start, double step, double max, double result[2], EvaluationAtAbscissa evaluation, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const;
  double brentRoot(const char * symbol, double ax, double bx, double precision, EvaluationAtAbscissa evaluation, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const;
};

}

#endif
