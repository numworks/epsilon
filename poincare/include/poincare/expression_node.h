#ifndef POINCARE_EXPRESSION_NODE_H
#define POINCARE_EXPRESSION_NODE_H

#include <poincare/tree_node.h>
#include <poincare/evaluation.h>
#include <poincare/layout.h>
#include <poincare/context.h>
#include <stdint.h>

namespace Poincare {

/* Methods preceded by '*!*' interfere with the expression pool, which can make
 * 'this' outdated. They should only be called in a wrapper on Expression. */

class ExpressionNode : public TreeNode {
  friend class AdditionNode;
  friend class DivisionNode;
  friend class NAryExpressionNode;
  friend class PowerNode;
  friend class SymbolNode;
public:
   enum class Type : uint8_t {
    Uninitialized = 0,
    Undefined = 1,
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
  /*!*/ virtual Expression replaceSymbolWithExpression(char symbol, Expression & expression);
  /*!*/ virtual Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit);
  virtual int polynomialDegree(char symbolName) const;
  /*!*/ virtual int getPolynomialCoefficients(char symbolName, Expression coefficients[]) const;
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
  virtual int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const;

  /* Layout Helper */
  virtual Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const = 0;

  /* Evaluation Helper */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  constexpr static int k_maxNumberOfSteps = 10000;
  virtual Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const = 0;
  virtual Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const = 0;

  /* Simplification */
  /*!*/ virtual Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  /*!*/ virtual Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
  /* Return a clone of the denominator part of the expression */
  /*!*/ virtual Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const;

  /* Hierarchy */
  ExpressionNode * childAtIndex(int i) const override { return static_cast<ExpressionNode *>(TreeNode::childAtIndex(i)); }
  virtual void setChildrenInPlace(Expression other);

protected:
  /* Hierarchy */
  ExpressionNode * parent() const override { return static_cast<ExpressionNode *>(TreeNode::parent()); }
  Direct<ExpressionNode> children() const { return Direct<ExpressionNode>(this); }
};

}

#endif
