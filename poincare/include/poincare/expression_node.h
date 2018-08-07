#ifndef POINCARE_EXPRESSION_NODE_H
#define POINCARE_EXPRESSION_NODE_H

#include <poincare/serializable_node.h>
#include <poincare/evaluation.h>
#include <poincare/layout_reference.h>
#include <poincare/context.h>
#include <stdint.h>

namespace Poincare {

/* Methods preceded by a '*!*' interfere with the expression pools which can
 * make 'this' outdated. They should only be call only be call in a wrapper on
 * ExpressionRef. */

class ExpressionNode : public SerializableNode {
  friend class ApproximationEngine;
  friend class SymbolNode;
  friend class NAryExpressionNode;
public:
   enum class Type : uint8_t {
    AllocationFailure = 0,
    Undefined = 1,
    Integer = 2,
    Rational,
    Decimal,
    Float,
    Infinity,
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
    EmptyExpression
   };

  /* Allocation failure */
  static TreeNode * FailedAllocationStaticNode();
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  /* Poor man's RTTI */
  virtual Type type() const = 0;

  /* Properties */
  enum class Sign {
    Negative = -1,
    Unknown = 0,
    Positive = 1
  };
  virtual Sign sign() const { return Sign::Unknown; }
  virtual bool isNumber() const { return false; }
  /*!*/ virtual ExpressionReference replaceSymbolWithExpression(char symbol, ExpressionReference expression);
  /*!*/ virtual ExpressionReference setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit);
  virtual int polynomialDegree(char symbolName) const;
  /*!*/ virtual int getPolynomialCoefficients(char symbolName, ExpressionReference coefficients[]) const;
  typedef bool (*isVariableTest)(char c);
  virtual int getVariables(isVariableTest isVariable, char * variables) const;
  virtual float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const;
  bool isOfType(Type * types, int length) const;

  // Useful to avoid parsing incorrect expressions as cos(2,3,4)
  virtual bool hasValidNumberOfOperands(int nbChildren) const { return numberOfChildren() == nbChildren; }

  /* Comparison */

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
  static int SimplificationOrder(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted = false);
  /* In the simplification order, most expressions are compared by only
   * comparing their types. However hierarchical expressions of same type would
   * compare their operands and thus need to reimplement
   * simplificationOrderSameType. Besides, operations that can be simplified
   * (ie +, *, ^, !) have specific rules to group like terms together and thus
   * reimplement simplificationOrderGreaterType. */
  virtual int simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const { return -1; }
  //TODO: What should be the implementation for complex?
  virtual int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const { return 0; }

  /* Layout Engine */
  virtual LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const = 0;

  /* Evaluation Engine */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  constexpr static int k_maxNumberOfSteps = 10000;
  virtual EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const = 0;
  virtual EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const = 0;

  /* Simplification */
  /*!*/ virtual ExpressionReference shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  /*!*/ virtual ExpressionReference shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);

protected:
  // Private methods used in simplification process
  /*!*/ virtual ExpressionReference cloneDenominator(Context & context, Preferences::AngleUnit angleUnit) const;

  /* Hierarchy */
  ExpressionNode * childAtIndex(int i) const override { return static_cast<ExpressionNode *>(SerializableNode::childAtIndex(i)); }
  ExpressionNode * parent() const override { return static_cast<ExpressionNode *>(SerializableNode::parent()); }
};

}

#endif
