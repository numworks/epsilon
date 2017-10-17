#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

class Context;
template<class T>
class Evaluation;

class Expression {
public:
  enum class Type : uint8_t {
    Undefined = 0,
    Rational = 1,
    Integer = 2, // delete
    Multiplication,
    Power,
    Addition,
    Factorial,
    AbsoluteValue,
    ArcCosine,
    ArcSine,
    ArcTangent,
    BinomialCoefficient,
    Ceiling,
    ComplexArgument,
    ConfidenceInterval,
    Conjugate,
    Cosine,
    Derivative,
    Determinant,
    Division,
    DivisionQuotient,
    DivisionRemainder,
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
    MatrixDimension,
    MatrixInverse,
    MatrixTrace,
    MatrixTranspose,
    NaperianLogarithm,
    NthRoot,
    Opposite,
    Parenthesis,
    PermuteCoefficient,
    PredictionInterval,
    Product,
    RealPart,
    Round,
    Sine,
    SquareRoot,
    Store,
    Subtraction,
    Sum,
    Tangent,
    Symbol,

    Complex,
    ComplexMatrix,
    ExpressionMatrix,
  };
  enum class FloatDisplayMode {
    Decimal = 0,
    Scientific = 1,
    Default = 2
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
  virtual ~Expression() = default;
  virtual Expression * clone() const = 0;
  /* sign equals:
   *  -1 means the expression is negative
   *  1 means the expression is positive
   *  0 means the sign is unknown */
  virtual int sign() const { return false; }
  virtual Expression * turnIntoPositive() { assert(false); return nullptr; }

  /* Poor man's RTTI */
  virtual Type type() const = 0;

  /* Circuit breaker */
  typedef bool (*CircuitBreaker)();
  static void setCircuitBreaker(CircuitBreaker cb);
  static bool shouldStopProcessing();

  /* Hierarchy */
  virtual const Expression * operand(int i) const = 0;
  virtual int numberOfOperands() const = 0;
  Expression * parent() const { return m_parent; }
  void setParent(Expression * parent) { m_parent = parent; }
  bool hasAncestor(const Expression * e) const;
  virtual void replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand = true) = 0;
  Expression * replaceWith(Expression * newOperand, bool deleteAfterReplace = true);
  virtual void swapOperands(int i, int j) = 0;
  //void removeFromParent();

  /* Sorting */
  /* compareTo returns:
   *   1 if this > e
   *   -1 if this < e
   *   0 if this == e */
  int compareTo(const Expression * e) const;

  /* Layout Engine */
  ExpressionLayout * createLayout(FloatDisplayMode floatDisplayMode = FloatDisplayMode::Default, ComplexFormat complexFormat = ComplexFormat::Default) const; // Returned object must be deleted
  virtual int writeTextInBuffer(char * buffer, int bufferSize) const = 0;

  /* Simplification */
  static void simplifyAndBeautify(Expression ** expressionAddress);
  Expression * simplify();
  // TODO: should be virtual pure
  virtual Expression * immediateSimplify();
  virtual Expression * immediateBeautify() { return this; };

  /* Evaluation Engine
   * The function evaluate creates a new expression and thus mallocs memory.
   * Do not forget to delete the new expression to avoid leaking. */
  template<typename T> Evaluation<T> * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> T approximate(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> static T approximate(const char * text, Context& context, AngleUnit angleUnit = AngleUnit::Default);
protected:
  /* Constructor */
  Expression() : m_parent(nullptr) {}
  /* Evaluation Engine */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  template<typename T> static T epsilon();
  /* Compare (== < and >) the type of the root node of 2 expressions.
   * This behavior makes sense for value-less nodes (addition, product, fraction
   * power, etc… For nodes with a value (Integer, Complex), this must be over-
   * -riden. */
private:
  /* Layout Engine */
  virtual ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const = 0;
  /* Evaluation Engine */
  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const = 0;
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const = 0;
  /* Simplification */
  // beautify cannot be dynamic as it changes the expression and THEN its new children
  Expression * beautify();
  /* Sorting */
  virtual int compareToGreaterTypeExpression(const Expression * e) const {
    return -1;
  }
  /* What should be the implementation of complex? */
  virtual int compareToSameTypeExpression(const Expression * e) const {
    return 0;
  }
  Expression * m_parent;
};

}

#endif
