#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/api.h>
#include <poincare/point_or_scalar.h>
#include <poincare/src/expression/dimension_type.h>
#include <poincare/src/memory/block.h>
#include <poincare/src/memory/k_tree_concept.h>

#include "old_expression.h"

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare {

class Symbol;

/* Aliases used to specify a JuniorExpression's type. TODO_PCJ: split them into
 * actual classes to prevent casting one into another. */

// Can be applied to different types of Expressions
using NewExpression = JuniorExpression;
// Can be layoutted (Not projected)
using UserExpression = NewExpression;
// Can be approximated without context
using NoContextExpression = NewExpression;
// Must have been projected
using ProjectedExpression = NoContextExpression;
// Must have been systematic simplified
using SystemExpression = ProjectedExpression;
// Can depend on a Variable and has been prepared for approximation
using SystemFunction = NoContextExpression;
// SystemFunction with Scalar approximation
using SystemFunctionScalar = SystemFunction;
// SystemFunction with Point approximation
using SystemFunctionPoint = SystemFunction;

class Dimension {
 public:
  Dimension(const NewExpression e, Context* context = nullptr);

  bool isScalar();
  bool isMatrix();
  bool isUnit();
  bool isBoolean();
  bool isPoint();

  bool isList();
  bool isListOfScalars();
  bool isListOfUnits();
  bool isListOfBooleans();
  bool isListOfPoints();

  bool isPointOrListOfPoints();

 private:
  Internal::DimensionType m_type;
  bool m_isList;
  bool m_dimensioned;
};

class JuniorExpressionNode final : public ExpressionNode {
  friend class JuniorExpression;

 public:
  JuniorExpressionNode(const Internal::Tree* tree, size_t treeSize);

  // PoolObject
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "JuniorExpression";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  // Properties
  Type otype() const override { return Type::JuniorExpression; }
  // TODO_PCJ: Plug in approximation
  // OMG::Troolean isPositive(Context* context) const override;
  // OMG::Troolean isNull(Context* context) const override;
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;
  int polynomialDegree(Context* context, const char* symbolName) const override;

  // Approximation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override;

  /* Similar to the previous one but bypass Evaluation */
  template <typename T>
  SystemExpression approximateToTree(
      const ApproximationContext& approximationContext) const;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits, Context* context) const;
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override {
    assert(false);
    return OExpression();
  }
  LayoutShape leftLayoutShape() const override {
    // TODO_PCJ: Remove
    assert(false);
    return LayoutShape::BoundaryPunctuation;
  }

 private:
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

  // PCJ
  const Internal::Tree* tree() const;

  Internal::Block m_blocks[0];
};

class JuniorExpression : public OExpression {
  friend class JuniorExpressionNode;

 public:
  JuniorExpression() {}
  JuniorExpression(const OExpression& other) { *this = other; }

  JuniorExpression(const API::UserExpression& ue) {
    *this = Builder(ue.tree());
  }

  NewExpression clone() const {
    return static_cast<NewExpression>(OExpression::clone());
  }

  static UserExpression Parse(const Internal::Tree* layout, Context* context,
                              bool addMissingParenthesis = true,
                              bool parseForAssignment = false);
  static UserExpression Parse(const char* layout, Context* context,
                              bool addMissingParenthesis = true,
                              bool parseForAssignment = false);
  static UserExpression ParseLatex(const char* latex, Context* context,
                                   bool addMissingParenthesis = true,
                                   bool parseForAssignment = false);

  static NewExpression Create(const Internal::Tree* structure,
                              Internal::ContextTrees ctx);
  static SystemExpression CreateReduce(const Internal::Tree* structure,
                                       Internal::ContextTrees ctx);
  operator const Internal::Tree*() const { return tree(); }
  // Builders from value.
  static SystemExpression Builder(int32_t n);
  template <typename T>
  static SystemExpression Builder(T x);
  template <typename T>
  static SystemExpression Builder(Coordinate2D<T> point);
  template <typename T>
  static SystemExpression Builder(PointOrScalar<T> pointOrScalar);

  template <Internal::KTrees::KTreeConcept T>
  static NewExpression Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }
  static NewExpression Builder(const Internal::Tree* tree);
  // Eat the tree
  static NewExpression Builder(Internal::Tree* tree);
  static NewExpression Juniorize(OExpression e);
  const Internal::Tree* tree() const {
    return isUninitialized() ? nullptr : node()->tree();
  }
  NewExpression childAtIndex(int i) const;
  int numberOfDescendants(bool includeSelf) const;
  ExpressionNode::Type type() const;
  bool isOfType(std::initializer_list<ExpressionNode::Type> types) const;

  NewExpression operator=(OExpression&& other) {
    *this = Juniorize(other);
    return *this;
  }

  NewExpression operator=(const OExpression& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorExpressionNode* node() const {
    return static_cast<JuniorExpressionNode*>(OExpression::node());
  }

  void cloneAndSimplifyAndApproximate(
      SystemExpression* simplifiedExpression,
      SystemExpression* approximatedExpression,
      const ReductionContext& reductionContext,
      bool approximateKeepingSymbols = false) const;
  SystemExpression cloneAndDeepReduceWithSystemCheckpoint(
      ReductionContext* reductionContext, bool* reduceFailure,
      bool approximateDuringReduction = false) const;

  UserExpression cloneAndSimplify(ReductionContext reductionContext,
                                  bool* reductionFailure = nullptr) const;
  SystemExpression cloneAndReduce(ReductionContext reductionContext) const;

  UserExpression cloneAndBeautify(
      const ReductionContext& reductionContext) const;

  SystemExpression getReducedDerivative(const char* symbolName,
                                        int derivationOrder = 1) const;
  /* Replace some UserSymbol into Var0 for approximateToPointOrScalarWithValue
   * Returns undef if the expression's dimension is not point or scalar.
   * If scalarsOnly = true, returns undef if it's a point or a list. */
  SystemFunction getSystemFunction(const char* symbolName,
                                   bool scalarsOnly = false) const;
  // Approximate to scalar replacing Var0 with value.
  template <typename T>
  T approximateToScalarWithValue(T x, int listElement = -1) const;
  // Approximate to PointOrScalar replacing Var0 with value.
  template <typename T>
  PointOrScalar<T> approximateToPointOrScalarWithValue(T x) const;

  // Return SystemExpression with sorted approximated elements.
  template <typename T>
  SystemExpression approximateListAndSort() const;
  // Return SystemExpression with undef list elements or points removed.
  SystemExpression removeUndefListElements() const;

  template <typename T>
  SystemExpression approximateToTree(
      const ApproximationContext& approximationContext) const {
    return node()->approximateToTree<T>(approximationContext);
  }
  /* Approximation Helper */
  template <typename T>
  static T ParseAndSimplifyAndApproximateToScalar(
      const char* text, Context* context,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);

  OExpression shallowReduce(ReductionContext reductionContext) {
    // TODO_PCJ
    assert(false);
    return OExpression();
  }
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);

  int getPolynomialReducedCoefficients(const char* symbolName,
                                       SystemExpression coefficients[],
                                       Context* context,
                                       Preferences::ComplexFormat complexFormat,
                                       Preferences::AngleUnit angleUnit,
                                       Preferences::UnitFormat unitFormat,
                                       SymbolicComputation symbolicComputation,
                                       bool keepDependencies = false) const;
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and returns the  polynomial degree.
   * It is supposed to be called on a reduced expression.
   * coefficients has up to 3 entries.  */
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                SystemExpression coefficients[]) const;

  char* toLatex(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits, Context* context,
                bool forceStripMargin = false, bool nested = false) const;

#if 1  // TODO_PCJ
  NewExpression replaceSymbolWithExpression(const SymbolAbstract& symbol,
                                            const NewExpression& expression,
                                            bool onlySecondTerm = false);

  typedef OMG::Troolean (*ExpressionTrinaryTest)(const NewExpression e,
                                                 Context* context,
                                                 void* auxiliary);
  struct IgnoredSymbols {
    NewExpression* head;
    void* tail;
  };
  bool recursivelyMatches(
      ExpressionTrinaryTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr,
      IgnoredSymbols* ignoredSymbols = nullptr) const;
  typedef bool (*ExpressionTest)(const NewExpression e, Context* context);
  bool recursivelyMatches(
      ExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  typedef bool (*SimpleExpressionTest)(const NewExpression e);
  bool recursivelyMatches(
      SimpleExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  typedef bool (*ExpressionTestAuxiliary)(const NewExpression e,
                                          Context* context, void* auxiliary);
  bool recursivelyMatches(
      ExpressionTestAuxiliary test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr) const;

  bool deepIsOfType(std::initializer_list<ExpressionNode::Type> types,
                    Context* context = nullptr) const;
  Dimension dimension(Context* context = nullptr) const;

  // Set of ExpressionTest that can be used with recursivelyMatches
  static bool IsUninitialized(const NewExpression e) {
    return e.isUninitialized();
  }
  static bool IsUndefined(const NewExpression e) { return e.isUndefined(); }
  static bool IsNAry(const NewExpression e) {
    return e.isOfType(
        {ExpressionNode::Type::Addition, ExpressionNode::Type::Multiplication});
  }
  static bool IsApproximate(const NewExpression e) {
    return e.isOfType({ExpressionNode::Type::Decimal,
                       ExpressionNode::Type::Float,
                       ExpressionNode::Type::Double});
  }
  static bool IsRandom(const NewExpression e) {
    assert(false);
    return false;
  }
  static bool IsMatrix(const NewExpression e, Context* context) {
    assert(false);
    return false;
  }
  static bool IsInfinity(const NewExpression e) {
    return e.isOfType({ExpressionNode::Type::Infinity});
  }
  static bool IsPercent(const NewExpression e) {
    return e.isOfType({ExpressionNode::Type::PercentSimple,
                       ExpressionNode::Type::PercentAddition});
  }
  static bool IsDiscontinuous(const NewExpression e, Context* context);
  static bool IsSymbolic(const NewExpression e) {
    assert(false);
    return false;
  }
  static bool IsSequence(const NewExpression e) {
    return e.type() == ExpressionNode::Type::Sequence;
  }
  static bool IsFactorial(const NewExpression e) {
    return e.type() == ExpressionNode::Type::Factorial;
  }

  typedef bool (*PatternTest)(const NewExpression& e, Context* context,
                              const char* symbol);
  static bool IsRationalFraction(const NewExpression& e, Context* context,
                                 const char* symbol) {
    assert(false);
    return false;
  }
  bool isLinearCombinationOfFunction(Context* context, PatternTest testFunction,
                                     const char* symbol) const {
    assert(false);
    return false;
  }

  bool isUndefined() const { return type() == ExpressionNode::Type::Undefined; }
  bool hasComplexI(
      Context* context,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  bool hasUnit(bool ignoreAngleUnits = false, bool* hasAngleUnits = nullptr,
               bool replaceSymbols = false, Context* ctx = nullptr) const;
  bool isPureAngleUnit() const;
  bool isInRadians(Context* context) const;
  bool involvesDiscontinuousFunction(Context* context) const;
#endif
#if 1
  /* TODO_PCJ: Remove those methods from PoolHandle once only JuniorExpression
   * remains. In the meantime, they are overriden there to assert false in case
   * they are still used. */
  /* Hierarchy */
  bool hasChild(PoolHandle t) const {
    assert(false);
    return false;
  }
  bool hasSibling(PoolHandle t) const {
    assert(false);
    return false;
  }
  bool hasAncestor(PoolHandle t, bool includeSelf) const {
    assert(false);
    return false;
  }
  PoolHandle commonAncestorWith(PoolHandle t,
                                bool includeTheseNodes = true) const {
    assert(false);
    return t;
  }
  void setNumberOfChildren(int numberOfChildren) { assert(false); }
  int indexOfChild(PoolHandle t) const {
    assert(false);
    return 0;
  }
  PoolHandle parent() const {
    assert(false);
    return *this;
  }
  void setParentIdentifier(uint16_t id) { assert(false); }
  void deleteParentIdentifier() { assert(false); }
  void deleteParentIdentifierInChildren() { assert(false); }
  void incrementNumberOfChildren(int increment = 1) { assert(false); }

  /* Hierarchy operations */
  // Replace
  void replaceChildInPlace(PoolHandle oldChild, PoolHandle newChild) {
    assert(false);
  }
  void replaceChildAtIndexInPlace(int oldChildIndex, PoolHandle newChild) {
    assert(false);
  }
  void replaceChildAtIndexWithGhostInPlace(int index) { assert(false); }
  void replaceChildWithGhostInPlace(PoolHandle t) { assert(false); }
  // Merge
  void mergeChildrenAtIndexInPlace(PoolHandle t, int i) { assert(false); }
  // Swap
  void swapChildrenInPlace(int i, int j) { assert(false); }
#endif
};

// TODO_PCJ: Actually implement methods. Assert its block type is Matrix
class Matrix final : public JuniorExpression {
 public:
  /* Cap the matrix's size for inverse and determinant computation.
   * TODO: Find another solution */
  constexpr static int k_maxNumberOfChildren = 100;
  static Matrix Builder();
  void setDimensions(int rows, int columns);
  bool isVector() const;
  int numberOfRows() const;
  int numberOfColumns() const;
  void addChildAtIndexInPlace(NewExpression t, int index,
                              int currentNumberOfChildren);
  NewExpression matrixChild(int i, int j);
  // rank returns -1 if the rank cannot be computed
  int rank(Context* context, bool forceCanonization = false);
};

class Point final : public JuniorExpression {
 public:
  static Point Builder(NewExpression x, NewExpression y);
  template <typename T>
  Coordinate2D<T> approximate2D(
      const ApproximationContext& approximationContext);
  Layout create2DLayout(Preferences::PrintFloatMode floatDisplayMode,
                        int significantDigits, Context* context) const;
};

// TODO_PCJ: Actually implement methods. Assert its block type is List
class List : public JuniorExpression {
 public:
  static List Builder();
  int numberOfChildren() const;

  void removeChildAtIndexInPlace(int i);
  void addChildAtIndexInPlace(NewExpression t, int index,
                              int currentNumberOfChildren);
};

class Boolean final : public JuniorExpression {
 public:
  bool value() const;
};

class Unit final {
 public:
  // Build default unit for given angleUnit preference.
  static NewExpression Builder(Preferences::AngleUnit angleUnit);
  static bool IsPureAngleUnit(NewExpression expression, bool radianOnly);
  static bool HasAngleDimension(NewExpression expression);
  static bool ShouldDisplayAdditionalOutputs(
      double value, OExpression unit, Preferences::UnitFormat unitFormat) {
    assert(false);
    return false;
  }
  static int SetAdditionalExpressions(OExpression units, double value,
                                      OExpression* dest, int availableLength,
                                      const ReductionContext& reductionContext,
                                      const OExpression exactOutput) {
    assert(false);
    return 0;
  }
};

}  // namespace Poincare

#endif
