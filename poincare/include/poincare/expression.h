#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>
#include <kandinsky.h>

namespace Poincare {

class Context;
template<class T>
class Evaluation;

class Expression {
public:
  enum class Type : uint8_t {
    AbsoluteValue,
    Addition,
    ArcCosine,
    ArcSine,
    ArcTangent,
    BinomialCoefficient,
    Ceiling,
    Complex,
    ComplexArgument,
    ConfidenceInterval,
    Conjugate,
    Cosine,
    Derivative,
    Determinant,
    DivisionQuotient,
    DivisionRemainder,
    Factorial,
    Float,
    Floor,
    FracPart,
    ExpressionMatrix,
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
    Logarithm,
    LeastCommonMultiple,
    MatrixDimension,
    MatrixInverse,
    MatrixTrace,
    MatrixTranspose,
    Multiplication,
    NaperianLogarithm,
    NthRoot,
    Evaluation,
    Opposite,
    PredictionInterval,
    Fraction,
    Parenthesis,
    PermuteCoefficient,
    Power,
    Product,
    ReelPart,
    Round,
    Sine,
    SquareRoot,
    Store,
    Sum,
    Subtraction,
    Symbol,
    Tangent,
  };
  enum class AngleUnit {
    Degree = 0,
    Radian = 1,
    Default = 2
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
  constexpr static int k_numberOfPrintedSignificantDigits = 7;
  constexpr static int k_numberOfStoredSignificantDigits = 15;
  static Expression * parse(char const * string);
  virtual ~Expression() = default;
  virtual bool hasValidNumberOfArguments() const = 0;
  ExpressionLayout * createLayout(FloatDisplayMode floatDisplayMode = FloatDisplayMode::Default, ComplexFormat complexFormat = ComplexFormat::Default) const; // Returned object must be deleted
  virtual const Expression * operand(int i) const = 0;
  virtual int numberOfOperands() const = 0;
  virtual Expression * clone() const = 0;
  virtual Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands = true) const = 0;

  // TODO: Consider std::unique_ptr - see https://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Ownership_and_Smart_Pointers

  /* This tests whether two expressions are the same, this takes into account
   * commutativity of operators.
   *
   * For example 3+5 is identical to 5+3 but is not identical to 8.
   */
  bool isIdenticalTo(const Expression * e) const;

  /* This tests whether two expressions are equivalent.
   * This is done by testing wheter they simplify to the same expression.
   *
   * For example:
   * - 3+5 and 4+4 are equivalent.
   * - (x+y)*z and x*z+y*z are equivalent.
   *
   * Here we assume that two equivalent expressions have the same
   * simplification, we don't really know whether that's the case,
   * nevertheless we are sure that if two expressions simplify to the same
   * expression they are indeed equivalent.
   */
  bool isEquivalentTo(Expression * e) const;

  /* Compare the value of two expressions.
   * This only make sense if the two values are of the same type
   */
  virtual bool valueEquals(const Expression * e) const;
  Expression * simplify() const;

  virtual Type type() const = 0;
  virtual bool isCommutative() const;

   typedef bool (*CircuitBreaker)(const Expression * e);
   static void setCircuitBreaker(CircuitBreaker cb);
   bool shouldStopProcessing() const;

  /* The function evaluate creates a new expression and thus mallocs memory.
   * Do not forget to delete the new expression to avoid leaking. */
  template<typename T> Evaluation<T> * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> T approximate(Context& context, AngleUnit angleUnit = AngleUnit::Default) const;
  template<typename T> static T approximate(const char * text, Context& context, AngleUnit angleUnit = AngleUnit::Default);
  virtual int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = k_numberOfStoredSignificantDigits) const;
protected:
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  template<typename T> static T epsilon();
private:
  virtual ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const = 0;
  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const = 0;
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const = 0;
  bool sequentialOperandsIdentity(const Expression * e) const;
  bool commutativeOperandsIdentity(const Expression * e) const;
  bool combinatoryCommutativeOperandsIdentity(const Expression * e,
      bool * operandMatched, int leftToMatch) const;
};

}

#endif
