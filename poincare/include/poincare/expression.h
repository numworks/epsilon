#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/layout_reference.h>
#include <poincare/print_float.h>
#include <complex>
extern "C" {
#include <assert.h>
}

namespace Poincare {

class Context;
class Rational;
template<class T> class Evaluation;

class Expression {
  template<typename T> friend class Complex;
  friend class Undefined;
  friend class Rational;
  friend class Decimal;
  friend class Multiplication;
  friend class Power;
  friend class Addition;
  friend class Factorial;
  friend class Factor;
  friend class Division;
  friend class Store;
  friend class Sine;
  friend class Cosine;
  friend class Tangent;
  friend class AbsoluteValue;
  friend class ArcCosine;
  friend class ArcSine;
  friend class ArcTangent;
  friend class BinomialCoefficient;
  friend class Ceiling;
  friend class ComplexArgument;
  friend class ConfidenceInterval;
  friend class Conjugate;
  friend class Derivative;
  friend class Determinant;
  friend class DivisionQuotient;
  friend class DivisionRemainder;
  friend class Equal;
  friend class Floor;
  friend class FracPart;
  friend class GreatCommonDivisor;
  friend class HyperbolicArcCosine;
  friend class HyperbolicArcSine;
  friend class HyperbolicArcTangent;
  friend class HyperbolicCosine;
  friend class HyperbolicSine;
  friend class HyperbolicTangent;
  friend class ImaginaryPart;
  friend class Integral;
  friend class LeastCommonMultiple;
  friend class Logarithm;
  friend class MatrixDimension;
  friend class MatrixInverse;
  friend class MatrixTrace;
  friend class MatrixTranspose;
  friend class NaperianLogarithm;
  friend class NthRoot;
  friend class Opposite;
  friend class Parenthesis;
  friend class PermuteCoefficient;
  friend class PredictionInterval;
  friend class Product;
  friend class RealPart;
  friend class Round;
  friend class SquareRoot;
  friend class Subtraction;
  friend class Sum;
  friend class Symbol;
  friend class Matrix;
  friend class SimplificationRoot;
  friend class Sequence;
  friend class Trigonometry;
  friend class ApproximationEngine;
  friend class SimplificationEngine;
  friend class LayoutEngine;
  friend class EmptyExpression;
  friend class Randint;
public:
  enum class Type : uint8_t {
    Undefined = 0,
    Rational = 1,
    Decimal,
    Approximation,
    Multiplication,
    Power,
    Addition,
    Factorial,
    Division,
    Store,
    Equal,
    Sine,
    Cosine,
    Tangent,
    AbsoluteValue,
    ArcCosine,
    ArcSine,
    ArcTangent,
    BinomialCoefficient,
    Ceiling,
    ComplexArgument,
    Conjugate,
    Derivative,
    Determinant,
    DivisionQuotient,
    DivisionRemainder,
    Factor,
    Floor,
    FracPart,
    GreatCommonDivisor,
    HyperbolicArcCosine,
    HyperbolicArcSine,
    HyperbolicArcTangent,
    HyperbolicCosine,
    HyperbolicSine,
    HyperbolicTangent,
    ImaginaryPart,
    Integral,
    LeastCommonMultiple,
    Logarithm,
    MatrixTrace,
    NaperianLogarithm,
    NthRoot,
    Opposite,
    Parenthesis,
    PermuteCoefficient,
    Product,
    Random,
    Randint,
    RealPart,
    Round,
    SquareRoot,
    Subtraction,
    Sum,
    Symbol,

    Matrix,
    ConfidenceInterval,
    MatrixDimension,
    MatrixInverse,
    MatrixTranspose,
    PredictionInterval,
    SimplificationRoot,
    EmptyExpression
  };

  /* Constructor & Destructor */
  static Expression * parse(char const * string);
  static void ReplaceSymbolWithExpression(Expression ** expressionAddress, char symbol, Expression * expression);
  virtual ~Expression() = default;
  virtual Expression * clone() const = 0;

  /* Poor man's RTTI */
  virtual Type type() const = 0;

  /* Circuit breaker */
  typedef bool (*CircuitBreaker)();
  static void setCircuitBreaker(CircuitBreaker cb);
  static bool shouldStopProcessing();

  /* Hierarchy */
  virtual const Expression * const * operands() const = 0;
  const Expression * operand(int i) const;
  Expression * editableOperand(int i) { return const_cast<Expression *>(operand(i)); }
  virtual int numberOfOperands() const = 0;

  Expression * replaceWith(Expression * newOperand, bool deleteAfterReplace = true);
  void replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand = true);
  void detachOperand(const Expression * e); // Removes an operand WITHOUT deleting it
  void detachOperands(); // Removes all operands WITHOUT deleting them
  void swapOperands(int i, int j);

  Expression * parent() const { return m_parent; }
  void setParent(Expression * parent) { m_parent = parent; }
  bool hasAncestor(const Expression * e) const;

  /* Properties */
  enum class Sign {
    Negative = -1,
    Unknown = 0,
    Positive = 1
  };
  bool isRationalZero() const;
  virtual Sign sign() const { return Sign::Unknown; }
  typedef bool (*ExpressionTest)(const Expression * e, Context & context);
  bool recursivelyMatches(ExpressionTest test, Context & context) const;
  bool isApproximate(Context & context) const;
  /* 'characteristicXRange' tries to assess the range on x where the expression
   * (considered as a function on x) has an interesting evolution. For example,
   * the period of the function on 'x' if it is periodic. If
   * the function is x-independent, the return value is 0.0f (because any
   * x-range is equivalent). If the function does not have an interesting range,
   * the return value is NAN.
   * NB: so far, we consider that the only way of building a periodic function
   * is to use sin/tan/cos(f(x)) with f a linear function. */
  virtual float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const;
  static bool IsMatrix(const Expression * e, Context & context);

  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynome
   * - the degree of the polynome otherwise */
  virtual int polynomialDegree(char symbolName) const;
  /* getVariables fills the table variables with the variable present in the
   * expression and returns the number of entries in filled in variables.
   * For instance getVariables('x+y+2*w/cos(4)') would result in
   * variables = « xyw »  and would return 3. If the final number of
   * variables would overflow the maxNumberOfVariables, getVariables return -1 */
  static constexpr int k_maxNumberOfVariables = 6;
  typedef bool (*isVariableTest)(char c);
  virtual int getVariables(isVariableTest isVariable, char * variables) const;
  /* getLinearCoefficients return false if the expression is not linear with
   * the variables hold in 'variables'. Otherwise, it fills 'coefficients' with
   * the coefficients of the variables hold in 'variables' (following the same
   * order) and 'constant' with the constant of the expression. */
  bool getLinearCoefficients(char * variables, Expression * coefficients[], Expression * constant[], Context & context, Preferences::AngleUnit angleUnit) const;
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and return polynomialDegree.
   * coefficients has up to 3 entries. It supposed to be called on Reduced
   * expression. */
  static constexpr int k_maxPolynomialDegree = 2;
  static constexpr int k_maxNumberOfPolynomialCoefficients = k_maxPolynomialDegree+1;
  int getPolynomialCoefficients(char symbolName, Expression * coefficients[], Context & context, Preferences::AngleUnit angleUnit) const;

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const Expression * e) const {
    /* We use the simplification order only because it is a already-coded total
     * order on expresssions. */
    return SimplificationOrder(this, e, true) == 0;
  }
  bool isEqualToItsApproximationLayout(Expression * approximation, int bufferSize, Preferences::AngleUnit angleUnit, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context & context);

  /* Layout Engine */
  virtual LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const = 0;
  virtual int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const = 0;

  /* Simplification */
  static Expression * ParseAndSimplify(const char * text, Context & context, Preferences::AngleUnit angleUnit);
  static void Simplify(Expression ** expressionAddress, Context & context, Preferences::AngleUnit angleUnit);
  static void Reduce(Expression ** expressionAddress, Context & context, Preferences::AngleUnit angleUnit, bool recursively = true);

  /* Evaluation Engine */
  /* The function approximate creates a new expression and thus mallocs memory.
   * Do not forget to delete the new expression to avoid leaking. */
  template<typename T> Expression * approximate(Context& context, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat) const;
  template<typename T> T approximateToScalar(Context& context, Preferences::AngleUnit angleUnit) const;
  template<typename T> static T approximateToScalar(const char * text, Context& context, Preferences::AngleUnit angleUnit);
  template<typename T> T approximateWithValueForSymbol(char symbol, T x, Context & context, Preferences::AngleUnit angleUnit) const;

  /* Expression roots/extrema solver*/
  struct Coordinate2D {
    double abscissa;
    double value;
  };
  Coordinate2D nextMinimum(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const;
  Coordinate2D nextMaximum(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const;
  double nextRoot(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const;
  Coordinate2D nextIntersection(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression) const;

  /* Evaluation engine */
  template<typename T> static T epsilon();
protected:
  /* Constructor */
  Expression() : m_parent(nullptr) {}
  /* Hierarchy */
  void detachOperandAtIndex(int i);
  /* Evaluation Engine */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  constexpr static int k_maxNumberOfSteps = 10000;

  /* Simplification */
  /* SimplificationOrder returns:
   *   1 if e1 > e2
   *   -1 if e1 < e2
   *   0 if e1 == e
   * The order groups like terms together to avoid quadratic complexity when
   * factorizing addition or multiplication. For example, it groups terms with
   * same bases together (ie Pi, Pi^3)  and with same non-rational factors
   * together (ie Pi, 2*Pi).
   * Because SimplificationOrder is a recursive call, we sometimes enable its
   * interruption to avoid freezing in the simplification process. */
  static int SimplificationOrder(const Expression * e1, const Expression * e2, bool canBeInterrupted = false);
private:
  virtual Expression * replaceSymbolWithExpression(char symbol, Expression * expression);
  /* Properties */
  virtual Expression * setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) { assert(false); return nullptr; }
  bool isOfType(Type * types, int length) const;
  virtual bool needParenthesisWithParent(const Expression * e) const;
  virtual int privateGetPolynomialCoefficients(char symbolName, Expression * coefficients[]) const;
  /* Comparison */
  /* In the simplification order, most expressions are compared by only
   * comparing their types. However hierarchical expressions of same type would
   * compare their operands and thus need to reimplement
   * simplificationOrderSameType. Besides, operations that can be simplified
   * (ie +, *, ^, !) have specific rules to group like terms together and thus
   * reimplement simplificationOrderGreaterType. */
  virtual int simplificationOrderGreaterType(const Expression * e, bool canBeInterrupted) const { return -1; }
  //TODO: What should be the implementation for complex?
  virtual int simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const { return 0; }
  /* Simplification */
  Expression * deepBeautify(Context & context, Preferences::AngleUnit angleUnit);
  Expression * deepReduce(Context & context, Preferences::AngleUnit angleUnit);
  // TODO: should be virtual pure
  virtual Expression * shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  virtual Expression * shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) { return this; };

  // Private methods used in simplification process
  virtual Expression * cloneDenominator(Context & context, Preferences::AngleUnit angleUnit) const {
    return nullptr;
  }
  /* Evaluation Engine */
  virtual Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const = 0;
  virtual Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const = 0;

  /* Expression roots/extrema solver*/
  constexpr static double k_solverPrecision = 1.0E-5;
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
  constexpr static double k_maxFloat = 1e100;
  typedef double (*EvaluationAtAbscissa)(char symbol, double abscissa, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression0, const Expression * expression1);
  Coordinate2D nextMinimumOfExpression(char symbol, double start, double step, double max, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression = nullptr, bool lookForRootMinimum = false) const;
  void bracketMinimum(char symbol, double start, double step, double max, double result[3], EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression = nullptr) const;
  Coordinate2D brentMinimum(char symbol, double ax, double bx, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression = nullptr) const;
  double nextIntersectionWithExpression(char symbol, double start, double step, double max, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression) const;
  void bracketRoot(char symbol, double start, double step, double max, double result[2], EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression) const;
  double brentRoot(char symbol, double ax, double bx, double precision, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression * expression) const;

  Expression * m_parent;
};

}

#endif
