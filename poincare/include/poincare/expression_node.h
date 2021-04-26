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
  friend class NAryInfixExpressionNode;
  friend class PowerNode;
  friend class SymbolNode;
public:
  // The types order is important here.
  enum class Type : uint8_t {
    Uninitialized = 0,
    Undefined = 1,
    Unreal,
    Rational,
    BasedInteger,
    Decimal,
    Double,
    Float,
    Infinity,
    /* When merging number nodes together, we do a linear scan which stops at
     * the first non-number child. */
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
    Sequence,
    SignFunction,
    SquareRoot,
    Subtraction,
    Sum,
    VectorDot,
    VectorNorm,
    /* When sorting the children of an expression, we assert that the following
     * nodes are at the end of the list : */
    // - Units
    Unit,
    // - Complexes
    ComplexCartesian,
    // - Any kind of matrices :
    ConfidenceInterval,
    MatrixDimension,
    MatrixIdentity,
    MatrixInverse,
    MatrixTranspose,
    MatrixRowEchelonForm,
    MatrixReducedRowEchelonForm,
    PredictionInterval,
    VectorCross,
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
    ReplaceAllSymbolsWithUndefined = 3, // Used in UnitConvert::shallowReduce
    DoNotReplaceAnySymbol = 4
  };
  enum class UnitConversion {
    None = 0,
    Default,
    InternationalSystem
  };
  enum class Sign {
    Negative = -1,
    Unknown = 0,
    Positive = 1
  };
  enum class NullStatus {
    Unknown = -1,
    NonNull = 0,
    Null = 1,
  };

  class ComputationContext {
  public:
    ComputationContext(
        Context * context,
        Preferences::ComplexFormat complexFormat,
        Preferences::AngleUnit angleUnit) :
      m_context(context),
      m_complexFormat(complexFormat),
      m_angleUnit(angleUnit)
    {}
    Context * context() { return m_context; }
    void setContext(Context * context) { m_context = context; }
    Preferences::ComplexFormat complexFormat() const { return m_complexFormat; }
    Preferences::AngleUnit angleUnit() const { return m_angleUnit; }
  private:
    Context * m_context;
    Preferences::ComplexFormat m_complexFormat;
    Preferences::AngleUnit m_angleUnit;
  };

  class ReductionContext : public ComputationContext {
  public:
    ReductionContext(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ReductionTarget target, SymbolicComputation symbolicComputation = SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, UnitConversion unitConversion = UnitConversion::Default) :
      ComputationContext(context, complexFormat, angleUnit),
      m_unitFormat(unitFormat),
      m_target(target),
      m_symbolicComputation(symbolicComputation),
      m_unitConversion(unitConversion)
    {}
    static ReductionContext NonInvasiveReductionContext(ReductionContext reductionContext) {
      return ReductionContext(
          reductionContext.context(),
          reductionContext.complexFormat(),
          reductionContext.angleUnit(),
          reductionContext.unitFormat(),
          reductionContext.target(),
          SymbolicComputation::DoNotReplaceAnySymbol,
          UnitConversion::None
        );
    }
    Preferences::UnitFormat unitFormat() const { return m_unitFormat; }
    ReductionTarget target() const { return m_target; }
    SymbolicComputation symbolicComputation() const { return m_symbolicComputation; }
    void setSymbolicComputation(SymbolicComputation symbolicComputation) { m_symbolicComputation = symbolicComputation; }
    UnitConversion unitConversion() const { return m_unitConversion; }
  private:
    Preferences::UnitFormat m_unitFormat;
    ReductionTarget m_target;
    SymbolicComputation m_symbolicComputation;
    UnitConversion m_unitConversion;
  };

  class ApproximationContext : public ComputationContext {
  public:
    ApproximationContext(
        Context * context,
        Preferences::ComplexFormat complexFormat,
        Preferences::AngleUnit angleUnit,
        bool withinReduce = false) :
      ComputationContext(context, complexFormat, angleUnit),
      m_withinReduce(withinReduce)
    {}
    ApproximationContext(ReductionContext reductionContext, bool withinReduce) :
      ApproximationContext(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), withinReduce) {}
    bool withinReduce() const { return m_withinReduce; }
  private:
    bool m_withinReduce;
  };

  virtual Sign sign(Context * context) const { return Sign::Unknown; }
  virtual NullStatus nullStatus(Context * context) const { return NullStatus::Unknown; }
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
  bool isOfType(Type * types, int length) const;

  virtual Expression removeUnit(Expression * unit); // Only reduced nodes should answer

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
  virtual Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const = 0;
  virtual Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const = 0;

  /* Simplification */
  /*!*/ virtual void deepReduceChildren(ReductionContext reductionContext);
  /*!*/ virtual void deepBeautifyChildren(ReductionContext reductionContext);
  /*!*/ virtual Expression shallowReduce(ReductionContext reductionContext);
  /* TODO: shallowBeautify takes a pointer to the reduction context, unlike
   * other methods. The pointer is needed to allow UnitConvert to modify the
   * context and prevent unit modifications (in Expression::deepBeautify, after
   * calling UnitConvert::shallowBeautify).
   * We should uniformize this behaviour and use pointers in other methods using
   * the reduction context. */
  /*!*/ virtual Expression shallowBeautify(ReductionContext * reductionContext);
  /*!*/ virtual bool derivate(ReductionContext, Expression symbol, Expression symbolValue);
  virtual Expression unaryFunctionDifferential(ReductionContext reductionContext);
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
