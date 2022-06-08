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
    Nonreal,
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
    PercentSimple,
    PercentAddition,
    Division,
    ConstantMaths,
    ConstantPhysics,
    Symbol,
    Store,
    UnitConvert,
    Equal,
    Superior,
    Inferior,
    SuperiorEqual,
    InferiorEqual,
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
    void setTarget(ReductionTarget target) { m_target = target; }
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
  bool isNumber() const;
  bool isRandom() const;
  bool isParameteredExpression() const;
  /* childAtIndexNeedsUserParentheses checks if parentheses are required by mathematical rules:
   * +(2,-1) --> 2+(-1)
   * *(+(2,1),3) --> (2+1)*3
   */
  virtual bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const { return false; }
  /*!*/ virtual Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  virtual int polynomialDegree(Context * context, const char * symbolName) const;
  /*!*/ virtual int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const;
  /*!*/ virtual Expression deepReplaceReplaceableSymbols(Context * context, bool * isCircular, int maxSymbolsToReplace, int parameteredAncestorsCount, SymbolicComputation symbolicComputation);
  typedef bool (*isVariableTest)(const char * c, Poincare::Context * context);
  virtual int getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const;
  bool isOfType(std::initializer_list<ExpressionNode::Type> types) const;
  /* This has a special behaviour for Multiplication, Power and Symbol.
   * See Addition::shallowBeautify - step 1. for more info. */
  virtual double degreeForSortingAddition(bool symbolsOnly) const { return symbolsOnly ? 0. : 1.; }

  bool hasMatrixOrListChild(Context * context) const;

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
  static int SimplificationOrder(const ExpressionNode * e1, const ExpressionNode * e2, bool ascending, bool ignoreParentheses = false);
  /* In the simplification order, most expressions are compared by only
   * comparing their types. However hierarchical expressions of same type would
   * compare their operands and thus need to reimplement
   * simplificationOrderSameType. Besides, operations that can be simplified
   * (ie +, *, ^, !) have specific rules to group like terms together and thus
   * reimplement simplificationOrderGreaterType. */
  virtual int simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const { return ascending ? -1 : 1; }
  virtual int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const;

  /* Layout Helper */
  virtual Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const = 0;

  /* Evaluation Helper */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  constexpr static int k_maxNumberOfSteps = 10000;
  virtual Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const = 0;
  virtual Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const = 0;

  /* Simplification */
  /*!*/ void deepReduceChildren(ReductionContext reductionContext);
  /*!*/ Expression deepBeautify(ReductionContext reductionContext);
  /*!*/ virtual Expression shallowReduce(ReductionContext reductionContext);
  /* TODO: shallowBeautify takes a pointer to the reduction context, unlike
   * other methods. The pointer is needed to allow UnitConvert to modify the
   * context and prevent unit modifications (in Expression::deepBeautify, after
   * calling UnitConvert::shallowBeautify).
   * We should uniformize this behaviour and use pointers in other methods using
   * the reduction context. */
  /*!*/ virtual Expression shallowBeautify(ReductionContext * reductionContext);
  /*!*/ virtual bool derivate(ReductionContext, Symbol symbol, Expression symbolValue);
  virtual Expression unaryFunctionDifferential(ReductionContext reductionContext);
  /* Return a clone of the denominator part of the expression */
  /*!*/ Expression denominator(ExpressionNode::ReductionContext reductionContext) const;
  /* LayoutShape is used to check if the multiplication sign can be omitted between two expressions.
   * It depends on the "layout syle" of the on the right of the left expression */
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
  ExpressionNode * childAtIndex(int i) const { return static_cast<ExpressionNode *>(TreeNode::childAtIndex(i)); }
#ifndef NDEBUG
  virtual void setChildrenInPlace(Expression other);
#else
  void setChildrenInPlace(Expression other);
#endif

protected:
  /* Hierarchy */
  ExpressionNode * parent() const { return static_cast<ExpressionNode *>(TreeNode::parent()); }
  Direct<ExpressionNode> children() const { return Direct<ExpressionNode>(this); }
};

// Helper to create a node
template<typename T, int N, typename Parent>
class Node : public Parent {
public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<ExpressionNode, Parent>::value);
#endif
  const char * functionHelperName() const override { return T::functionName; }
  size_t size() const override { return sizeof(T); }
  int numberOfChildren() const override { return N; };

  // TreeNode
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << T::nodeName;
  }
#endif
  // Evaluation
/*  Evaluation<float> approximate(ExpressionNode::SinglePrecision p, ExpressionNode::ApproximationContext approximationContext) const override { return T::template templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(ExpressionNode::DoublePrecision p, ExpressionNode::ApproximationContext approximationContext) const override { return T::template templatedApproximate<double>(approximationContext); }
*/
};

}

#endif
