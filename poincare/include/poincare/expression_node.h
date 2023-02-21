#ifndef POINCARE_EXPRESSION_NODE_H
#define POINCARE_EXPRESSION_NODE_H

#include <poincare/computation_context.h>
#include <poincare/context.h>
#include <poincare/evaluation.h>
#include <poincare/layout.h>
#include <poincare/tree_node.h>
#include <poincare/trinary_boolean.h>
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
    Nonreal,
    Boolean,
    Rational,
    BasedInteger,
    MixedFraction,
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
    PercentSimple,
    PercentAddition,
    Division,
    ConstantMaths,
    ConstantPhysics,
    Symbol,
    Store,
    UnitConvert,
    LogicalOperatorNot,
    BinaryLogicalOperator,
    Comparison,
    Sine,
    Cosecant,
    Cosine,
    Secant,
    Tangent,
    Cotangent,
    AbsoluteValue,
    ArcCosecant,
    ArcCosine,
    ArcCotangent,
    ArcSecant,
    ArcSine,
    ArcTangent,
    BinomialCoefficient,
    Ceiling,
    ComplexArgument,
    Conjugate,
    Dependency,
    Derivative,
    Determinant,
    DistributionDispatcher,
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
    LeastCommonMultiple,
    ListElement,
    ListMaximum,
    ListMean,
    ListMedian,
    ListMinimum,
    ListProduct,
    ListSampleStandardDeviation,
    ListStandardDeviation,
    ListSum,
    ListVariance,
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
    RandintNoRepeat,
    RealPart,
    Round,
    Sequence,
    SignFunction,
    SquareRoot,
    Subtraction,
    Sum,
    VectorDot,
    VectorNorm,
    PiecewiseOperator,
    /* When sorting the children of an expression, we assert that the following
     * nodes are at the end of the list : */
    // - Units
    Unit,
    // - Complexes
    ComplexCartesian,
    // - Any kind of lists :
    List,
    ListSequence,
    ListSort,
    ListSlice,
    // - Any kind of matrices :
    Dimension,
    MatrixIdentity,
    MatrixInverse,
    MatrixTranspose,
    MatrixRowEchelonForm,
    MatrixReducedRowEchelonForm,
    VectorCross,
    Matrix,
    EmptyExpression
  };

  /* Poor man's RTTI */
  virtual Type type() const = 0;

  /* Properties */
  virtual TrinaryBoolean isPositive(Context* context) const {
    return TrinaryBoolean::Unknown;
  }
  virtual TrinaryBoolean isNull(Context* context) const {
    return TrinaryBoolean::Unknown;
  }
  bool isNumber() const;
  bool isRandom() const;
  bool isParameteredExpression() const;
  bool isCombinationOfUnits() const;
  bool isUndefined() const {
    return isOfType({Type::Undefined, Type::Nonreal});
  }
  int numberOfNumericalValues() const;
  /* childAtIndexNeedsUserParentheses checks if parentheses are required by
   * mathematical rules:
   * +(2,-1) --> 2+(-1)
   * *(+(2,1),3) --> (2+1)*3
   */
  virtual bool childAtIndexNeedsUserParentheses(const Expression& child,
                                                int childIndex) const {
    return false;
  }
  /*!*/ virtual Expression replaceSymbolWithExpression(
      const SymbolAbstract& symbol, const Expression& expression);
  virtual int polynomialDegree(Context* context, const char* symbolName) const;
  /*!*/ virtual int getPolynomialCoefficients(Context* context,
                                              const char* symbolName,
                                              Expression coefficients[]) const;
  virtual bool involvesCircularity(Context* context, int maxDepth,
                                   const char** visitedFunctions,
                                   int numberOfVisitedFunctions);
  /*!*/ virtual Expression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation);
  typedef bool (*isVariableTest)(const char* c, Poincare::Context* context);
  virtual int getVariables(Context* context, isVariableTest isVariable,
                           char* variables, int maxSizeVariable,
                           int nextVariableIndex) const;
  bool isOfType(std::initializer_list<ExpressionNode::Type> types) const;
  /* This has a special behaviour for Multiplication, Power and Symbol.
   * See Addition::shallowBeautify - step 1. for more info. */
  virtual double degreeForSortingAddition(bool symbolsOnly) const {
    return symbolsOnly ? 0. : 1.;
  }

  bool hasMatrixOrListChild(Context* context, bool isReduced = true) const;

  virtual Expression removeUnit(
      Expression* unit);  // Only reduced nodes should answer

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
  static int SimplificationOrder(const ExpressionNode* e1,
                                 const ExpressionNode* e2, bool ascending,
                                 bool ignoreParentheses = false);
  /* In the simplification order, most expressions are compared by only
   * comparing their types. However hierarchical expressions of same type would
   * compare their operands and thus need to reimplement
   * simplificationOrderSameType. Besides, operations that can be simplified
   * (ie +, *, ^, !) have specific rules to group like terms together and thus
   * reimplement simplificationOrderGreaterType. */
  virtual int simplificationOrderGreaterType(const ExpressionNode* e,
                                             bool ascending,
                                             bool ignoreParentheses) const {
    return ascending ? -1 : 1;
  }
  virtual int simplificationOrderSameType(const ExpressionNode* e,
                                          bool ascending,
                                          bool ignoreParentheses) const;

  /* Layout Helper */
  virtual Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits,
                              Context* context) const = 0;

  /* Evaluation Helper */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  constexpr static int k_maxNumberOfSteps = 10000;
  virtual Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const = 0;
  virtual Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const = 0;

  /* Simplification */
  /*!*/ void deepReduceChildren(const ReductionContext& reductionContext);
  /*!*/ Expression deepBeautify(const ReductionContext& reductionContext);
  /*!*/ virtual Expression shallowReduce(
      const ReductionContext& reductionContext);
  /* TODO: shallowBeautify takes a pointer to the reduction context, unlike
   * other methods. The pointer is needed to allow UnitConvert to modify the
   * context and prevent unit modifications (in Expression::deepBeautify, after
   * calling UnitConvert::shallowBeautify).
   * We should uniformize this behaviour and use pointers in other methods using
   * the reduction context. */
  /*!*/ virtual Expression shallowBeautify(
      const ReductionContext& reductionContext);
  /*!*/ virtual bool derivate(const ReductionContext& reductionContext,
                              Symbol symbol, Expression symbolValue);
  virtual Expression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
  /* Return a clone of the denominator part of the expression */
  /*!*/ Expression denominator(const ReductionContext& reductionContext) const;
  /* LayoutShape is used to check if the multiplication sign can be omitted
   * between two expressions. It depends on the "layout syle" of the on the
   * right of the left expression */
  enum class LayoutShape {
    Decimal,
    Integer,
    OneLetter,
    MoreLetters,
    BoundaryPunctuation,  // ( [ ∫
    Brace,
    Root,
    NthRoot,
    Fraction,
    RightOfPower,
    Default
  };
  virtual LayoutShape leftLayoutShape() const = 0;
  virtual LayoutShape rightLayoutShape() const { return leftLayoutShape(); }

  /* Hierarchy */
  ExpressionNode* childAtIndex(int i) const {
    assert(i >= 0 && i < numberOfChildren());
    return static_cast<ExpressionNode*>(TreeNode::childAtIndex(i));
  }

#if ASSERTIONS
  virtual void setChildrenInPlace(Expression other);
#else
  void setChildrenInPlace(Expression other);
#endif

 protected:
  /* Hierarchy */
  ExpressionNode* parent() const {
    return static_cast<ExpressionNode*>(TreeNode::parent());
  }
  Direct<ExpressionNode> children() const {
    return Direct<ExpressionNode>(this);
  }
};

}  // namespace Poincare

#endif
