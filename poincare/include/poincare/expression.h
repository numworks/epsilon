#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>

namespace Poincare {

class Context;
template<class T>
class Evaluation;

class Expression {
public:
  enum class Type : uint8_t {
    Integer = 0,
    Complex,
    Symbol,
    Parenthesis,
    Opposite,
    Addition,
    Subtraction,
    Multiplication,
    Fraction,
    Power,
    Sum,
    Product,
    DivisionQuotient,
    DivisionRemainder,
    GreatCommonDivisor,
    LeastCommonMultiple,
    Floor,
    Ceiling,
    Round,
    FracPart,
    AbsoluteValue,
    Factorial,
    ImaginaryPart,
    ReelPart,
    ComplexArgument,
    Conjugate,
    Logarithm,
    NaperianLogarithm,
    SquareRoot,
    NthRoot,
    Cosine,
    Sine,
    Tangent,
    ArcCosine,
    ArcSine,
    ArcTangent,
    HyperbolicCosine,
    HyperbolicSine,
    HyperbolicTangent,
    HyperbolicArcCosine,
    HyperbolicArcSine,
    HyperbolicArcTangent,
    Derivative,
    Integral,
    BinomialCoefficient,
    PermuteCoefficient,
    ConfidenceInterval,
    PredictionInterval,
    ExpressionMatrix,
    ComplexMatrix,
    MatrixDimension,
    MatrixInverse,
    MatrixTrace,
    MatrixTranspose,
    Determinant,
    Store,
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

  /* Poor man's RTTI */
  virtual Type type() const = 0;

   /* Circuit breaker */
   typedef bool (*CircuitBreaker)();
   static void setCircuitBreaker(CircuitBreaker cb);
   static bool shouldStopProcessing();

  /* Hierarchy */
  virtual bool hasValidNumberOfArguments() const;
  virtual const Expression * operand(int i) const = 0;
  virtual int numberOfOperands() const = 0;
  virtual Expression * clone() const = 0;

  /* Layout Engine */
  ExpressionLayout * createLayout(FloatDisplayMode floatDisplayMode = FloatDisplayMode::Default, ComplexFormat complexFormat = ComplexFormat::Default) const; // Returned object must be deleted

  /* Commutative rule */
  virtual bool isCommutative() const = 0;
  virtual void sort();
  /* This tests whether two expressions are the =, <, >, it heavily relies on the
   * fact that operands are sorted.
   */
  int comparesTo(const Expression * e) const;
  //Expression * simplify() const;

  /* Evaluation Engine
   * The function evaluate creates a new expression and thus mallocs memory.
   * Do not forget to delete the new expression to avoid leaking. */
  template<typename T> Evaluation<T> * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> T approximate(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> static T approximate(const char * text, Context& context, AngleUnit angleUnit = AngleUnit::Default);
protected:
  /* Evaluation Engine */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  template<typename T> static T epsilon();
  /* Compare (== < and >) the type of the root node of 2 expressions.
   * This behavior makes sense for value-less nodes (addition, product, fraction
   * power, etc… For nodes with a value (Integer, Complex), this must be over-
   * -riden. */
  virtual int nodeComparesTo(const Expression * e) const;
private:
  /* Layout Engine */
  virtual ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const = 0;
  /* Evaluation Engine */
  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const = 0;
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const = 0;

};

}

#endif
