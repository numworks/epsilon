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

class SymbolAbstract;
class Symbol;
class ComplexCartesian;

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
    Unreal,
    Rational,
    BasedInteger,
    Decimal,
    Float,
    Infinity,
    Multiplication,
    Power,
    Addition,
    Factorial,
    Division,
    Constant,
    Symbol,
    Store,
    UnitConvert,
    Equal,
    Sine,
    Cosine,
    Tangent,
    AbsoluteValue,
    ArcCosine,
    ArcSine,
    ArcTangent,
    BinomCDF,
    BinomialCoefficient,
    BinomPDF,
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
    Function,
    GreatCommonDivisor,
    HyperbolicArcCosine,
    HyperbolicArcSine,
    HyperbolicArcTangent,
    HyperbolicCosine,
    HyperbolicSine,
    HyperbolicTangent,
    ImaginaryPart,
    Integral,
    InvBinom,
    InvNorm,
    LeastCommonMultiple,
    Logarithm,
    MatrixTrace,
    NaperianLogarithm,
    NormCDF,
    NormCDF2,
    NormPDF,
    NthRoot,
    Opposite,
    Parenthesis,
    PermuteCoefficient,
    Product,
    Random,
    Randint,
    RealPart,
    Round,
    SignFunction,
    SquareRoot,
    Subtraction,
    Sum,

    Unit,
    ComplexCartesian,

    ConfidenceInterval,
    MatrixDimension,
    MatrixIdentity,
    MatrixInverse,
    MatrixTranspose,
    PredictionInterval,
    Matrix,
    EmptyExpression
   };

  /* Poor man's RTTI */
  virtual Type type() const = 0;

  /* Properties */
  enum class ReductionTarget {
    /* Minimal reduction: this at least reduces rationals operations as
     * "1-0.3-0.7 --> 0" */
    SystemForApproximation = 0,
    /* Expansion of Newton multinome to be able to identify polynoms */
    SystemForAnalysis,
    /* Additional features as:
     * - factorizing on a common denominator
     * - turning complex expression into the form a+ib
     * - identifying tangent in cos/sin polynoms ... */
    User
  };
  enum class SymbolicComputation {
    ReplaceAllSymbolsWithDefinitionsOrUndefined = 0,
    ReplaceAllDefinedSymbolsWithDefinition = 1,
    ReplaceDefinedFunctionsWithDefinitions = 2,
    ReplaceAllSymbolsWithUndefinedAndDoNotReplaceUnits = 3, // Used in UnitConvert::shallowReduce
    ReplaceAllSymbolsWithUndefinedAndReplaceUnits = 4 // Used in UnitConvert::shallowReduce
  };
  enum class Sign {
    Negative = -1,
    Unknown = 0,
    Positive = 1
  };

  class ReductionContext {
  public:
    ReductionContext(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, SymbolicComputation symbolicComputation = SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) :
      m_context(context),
      m_complexFormat(complexFormat),
      m_angleUnit(angleUnit),
      m_target(target),
      m_symbolicComputation(symbolicComputation)
    {}
    Context * context() { return m_context; }
    Preferences::ComplexFormat complexFormat() const { return m_complexFormat; }
    Preferences::AngleUnit angleUnit() const { return m_angleUnit; }
    ReductionTarget target() const { return m_target; }
    SymbolicComputation symbolicComputation() const { return m_symbolicComputation; }
  private:
    Context * m_context;
    Preferences::ComplexFormat m_complexFormat;
    Preferences::AngleUnit m_angleUnit;
    ReductionTarget m_target;
    SymbolicComputation m_symbolicComputation;
  };

  virtual Sign sign(Context * context) const { return Sign::Unknown; }
  virtual bool isNumber() const { return false; }
  virtual bool isRandom() const { return false; }
  virtual bool isParameteredExpression() const { return false; }
  /* childAtIndexNeedsUserParentheses checks if parentheses are required by mathematical rules:
   * +(2,-1) --> 2+(-1)
   * *(+(2,1),3) --> (2+1)*3
   */
  virtual bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const { return false; }
  /*!*/ virtual Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  /*!*/ virtual Expression setSign(Sign s, ReductionContext reductionContext);
  virtual int polynomialDegree(Context * context, const char * symbolName) const;
  /*!*/ virtual int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const;
  /*!*/ virtual Expression deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount);
  typedef bool (*isVariableTest)(const char * c, Poincare::Context * context);
  virtual int getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const;
  virtual float characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const;
  bool isOfType(Type * types, int length) const;

  virtual Expression getUnit() const; // Only reduced nodes should answer

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
  static int SimplificationOrder(const ExpressionNode * e1, const ExpressionNode * e2, bool ascending, bool canBeInterrupted = false, bool ignoreParentheses = false);
  /* In the simplification order, most expressions are compared by only
   * comparing their types. However hierarchical expressions of same type would
   * compare their operands and thus need to reimplement
   * simplificationOrderSameType. Besides, operations that can be simplified
   * (ie +, *, ^, !) have specific rules to group like terms together and thus
   * reimplement simplificationOrderGreaterType. */
  virtual int simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const { return ascending ? -1 : 1; }
  virtual int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const;

  /* Layout Helper */
  virtual Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const = 0;

  /* Evaluation Helper */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  constexpr static int k_maxNumberOfSteps = 10000;
  virtual Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const = 0;
  virtual Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const = 0;

  /* Simplification */
  /*!*/ virtual void deepReduceChildren(ReductionContext reductionContext);
  /*!*/ virtual Expression shallowReduce(ReductionContext reductionContext);
  /*!*/ virtual Expression shallowBeautify(ReductionContext reductionContext);
  /* Return a clone of the denominator part of the expression */
  /*!*/ virtual Expression denominator(ExpressionNode::ReductionContext reductionContext) const;
  /* LayoutShape is used to check if the multiplication sign can be omitted between two expressions. It depends on the "layout syle" of the on the right of the left expression */
  enum class LayoutShape {
    Decimal,
    Integer,
    BinaryHexadecimal,
    OneLetter,
    MoreLetters,
    BoundaryPunctuation, // ( [ ∫
    Root,
    NthRoot,
    Fraction,
    RightOfPower
  };
  virtual LayoutShape leftLayoutShape() const = 0;
  virtual LayoutShape rightLayoutShape() const { return leftLayoutShape(); }

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
