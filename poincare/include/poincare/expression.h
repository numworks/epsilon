#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>
#include <poincare/print_float.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

class Context;
template<class T> class Complex;
class Rational;

class Expression {
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
  friend class Complex<float>;
  friend class Complex<double>;

public:
  enum class Type : uint8_t {
    Undefined = 0,
    Rational = 1,
    Decimal,
    Multiplication,
    Power,
    Addition,
    Factorial,
    Division,
    Store,
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

    Complex,
    Matrix,
    ConfidenceInterval,
    MatrixDimension,
    MatrixInverse,
    MatrixTranspose,
    PredictionInterval,
    SimplificationRoot,
  };
  enum class ComplexFormat {
    Cartesian = 0,
    Polar = 1,
    Default = 2
  };
  enum class AngleUnit {
    Degree = 0,
    Radian = 1,
    Default = 2
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
  virtual float characteristicXRange(Context & context, AngleUnit angleUnit = AngleUnit::Default) const;
  static bool IsMatrix(const Expression * e, Context & context);

  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynome
   * - the degree of the polynome otherwise */
  virtual int polynomialDegree(char symbolName) const;

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const Expression * e) const {
    /* We use the simplification order only because it is a already-coded total
     * order on expresssions. */
    return SimplificationOrder(this, e, true) == 0;
  }

  /* Layout Engine */
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode = PrintFloat::Mode::Default, ComplexFormat complexFormat = ComplexFormat::Default) const; // Returned object must be deleted
  virtual int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const = 0;

  /* Simplification */
  static Expression * ParseAndSimplify(const char * text, Context & context, AngleUnit angleUnit = AngleUnit::Default);
  static void Simplify(Expression ** expressionAddress, Context & context, AngleUnit angleUnit = AngleUnit::Default);

  /* Evaluation Engine
   * The function evaluate creates a new expression and thus mallocs memory.
   * Do not forget to delete the new expression to avoid leaking. */
  template<typename T> Expression * approximate(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> T approximateToScalar(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> static T approximateToScalar(const char * text, Context& context, AngleUnit angleUnit = AngleUnit::Default);
protected:
  /* Constructor */
  Expression() : m_parent(nullptr) {}
  /* Hierarchy */
  void detachOperandAtIndex(int i);
  /* Evaluation Engine */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  template<typename T> static T epsilon();
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
  virtual Expression * setSign(Sign s, Context & context, AngleUnit angleUnit) { assert(false); return nullptr; }
  bool isOfType(Type * types, int length) const;
  virtual bool needParenthesisWithParent(const Expression * e) const;
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
  /* Layout Engine */
  virtual ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const = 0;
  /* Simplification */
  static void Reduce(Expression ** expressionAddress, Context & context, AngleUnit angleUnit, bool recursively = true);
  Expression * deepBeautify(Context & context, AngleUnit angleUnit);
  Expression * deepReduce(Context & context, AngleUnit angleUnit);
  // TODO: should be virtual pure
  virtual Expression * shallowReduce(Context & context, AngleUnit angleUnit);
  virtual Expression * shallowBeautify(Context & context, AngleUnit angleUnit) { return this; };

  // Private methods used in simplification process
  virtual Expression * cloneDenominator(Context & context, AngleUnit angleUnit) const {
    return nullptr;
  }
  /* Evaluation Engine */
  virtual Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const = 0;
  virtual Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const = 0;

  Expression * m_parent;
};

}

#endif
