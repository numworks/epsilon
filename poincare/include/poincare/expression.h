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
    AbsoluteValue = 0,
    Addition,
    ArcCosine,
    ArcSine,
    ArcTangent,
    BinomialCoefficient,
    Ceiling,
    Complex,
    ComplexArgument,
    ComplexMatrix,
    ConfidenceInterval,
    Conjugate,
    Cosine,
    Derivative,
    Determinant,
    Division,
    DivisionQuotient,
    DivisionRemainder,
    ExpressionMatrix,
    Factorial,
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
    Integer,
    Integral,
    LeastCommonMultiple,
    Logarithm,
    MatrixDimension,
    MatrixInverse,
    MatrixTrace,
    MatrixTranspose,
    Multiplication,
    NaperianLogarithm,
    NthRoot,
    Opposite,
    Parenthesis,
    PermuteCoefficient,
    Power,
    PredictionInterval,
    Product,
    RealPart,
    Round,
    Sine,
    SquareRoot,
    Store,
    Subtraction,
    Sum,
    Symbol,
    Tangent,
    Undefined = 255
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

  /* Poor man's RTTI */
  virtual Type type() const = 0;
  // Allow to narrow down some nodes when querying the Expression tree.
  // Currently implemented by Symbol and Integer
  virtual int identifier() const { assert(false); return 0; }

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
  void replaceWith(Expression * newOperand);
  virtual void swapOperands(int i, int j) = 0;
  void removeFromParent();

  /* Sorting */
  virtual bool isCommutative() const { return false; }
  virtual void sort();
  /* compareTo returns:
   *   1 if this > e
   *   -1 if this < e
   *   0 if this == e */
  virtual int compareTo(const Expression * e) const;

  /* Layout Engine */
  ExpressionLayout * createLayout(FloatDisplayMode floatDisplayMode = FloatDisplayMode::Default, ComplexFormat complexFormat = ComplexFormat::Default) const; // Returned object must be deleted

  static void simplify(Expression ** e);

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
private:
  Expression * m_parent;
};

}

#endif
