#ifndef POINCARE_JUNIOR_EXPRESSION_H
#define POINCARE_JUNIOR_EXPRESSION_H

#include <poincare/old_expression.h>
#include <poincare_junior/src/memory/block.h>

namespace PoincareJ {
class Tree;
struct ContextTrees;
}  // namespace PoincareJ

namespace Poincare {

class Symbol;

class JuniorExpressionNode final : public ExpressionNode {
  friend class JuniorExpression;

 public:
  JuniorExpressionNode(const PoincareJ::Tree* tree, size_t treeSize);

  // TreeNode
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
  // TODO PCJ: Plug in approximation
  // TrinaryBoolean isPositive(Context* context) const override;
  // TrinaryBoolean isNull(Context* context) const override;
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

  // Approximation
  template <typename T>
  static std::complex<T> computeOnComplex(
      const std::complex<T> c, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit) {
    // TODO PCJ: Plug in approximation
    assert(false);
    return NAN;
  }
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override;

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
    // TODO PCJ: Remove
    assert(false);
    return LayoutShape::BoundaryPunctuation;
  }

 private:
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

  // PCJ
  const PoincareJ::Tree* tree() const;

  PoincareJ::Block m_blocks[0];
};

class JuniorExpression : public OExpression {
  friend class JuniorExpressionNode;

 public:
  JuniorExpression() {}
  JuniorExpression(const OExpression& other) { *this = other; }

  static JuniorExpression Parse(const PoincareJ::Tree* layout, Context* context,
                                bool addMissingParenthesis = true,
                                bool parseForAssignment = false);
  static JuniorExpression Parse(const char* layout, Context* context,
                                bool addMissingParenthesis = true,
                                bool parseForAssignment = false);

  static JuniorExpression Create(const PoincareJ::Tree* structure,
                                 PoincareJ::ContextTrees ctx);
  operator const PoincareJ::Tree*() { return tree(); }
  // Builders from value.
  static JuniorExpression Builder(int32_t n);
  template <typename T>
  static JuniorExpression Builder(T x);

  static JuniorExpression Builder(const PoincareJ::Tree* tree);
  // Eat the tree
  static JuniorExpression Builder(PoincareJ::Tree* tree);
  static JuniorExpression Juniorize(OExpression e);
  const PoincareJ::Tree* tree() const {
    assert(!isUninitialized());
    return node()->tree();
  }
  JuniorExpression childAtIndex(int i) const;
  ExpressionNode::Type type() const;
  bool isOfType(std::initializer_list<ExpressionNode::Type> types) const;

  JuniorExpression operator=(OExpression&& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorExpression operator=(const OExpression& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorExpressionNode* node() const {
    return static_cast<JuniorExpressionNode*>(OExpression::node());
  }

  void cloneAndSimplifyAndApproximate(
      JuniorExpression* simplifiedExpression,
      JuniorExpression* approximateExpression,
      const ReductionContext& reductionContext,
      bool approximateKeepingSymbols = false) const;
  JuniorExpression cloneAndDeepReduceWithSystemCheckpoint(
      ReductionContext* reductionContext, bool* reduceFailure,
      bool approximateDuringReduction = false) const;

  JuniorExpression cloneAndSimplify(ReductionContext reductionContext,
                                    bool* reductionFailure = nullptr) const;
  JuniorExpression cloneAndReduce(ReductionContext reductionContext) const;

  // This also reduces the expression. Approximation is in double.
  JuniorExpression cloneAndApproximateKeepingSymbols(
      ReductionContext reductionContext) const;

  OExpression shallowReduce(ReductionContext reductionContext) {
    // TODO PCJ
    assert(false);
    return OExpression();
  }
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);

  int getPolynomialReducedCoefficients(const char* symbolName,
                                       JuniorExpression coefficients[],
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
                                JuniorExpression coefficients[]) const;

  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynomial
   * - the degree of the polynomial otherwise */
  int polynomialDegree(Context* context, const char* symbolName) const;

#if 1  // TODO_PCJ
  JuniorExpression replaceSymbolWithExpression(
      const SymbolAbstract& symbol, const JuniorExpression& expression);

  typedef TrinaryBoolean (*ExpressionTrinaryTest)(const JuniorExpression e,
                                                  Context* context,
                                                  void* auxiliary);
  struct IgnoredSymbols {
    JuniorExpression* head;
    void* tail;
  };
  bool recursivelyMatches(
      ExpressionTrinaryTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr,
      IgnoredSymbols* ignoredSymbols = nullptr) const;
  typedef bool (*ExpressionTest)(const JuniorExpression e, Context* context);
  bool recursivelyMatches(
      ExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  typedef bool (*SimpleExpressionTest)(const JuniorExpression e);
  bool recursivelyMatches(
      SimpleExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  typedef bool (*ExpressionTestAuxiliary)(const JuniorExpression e,
                                          Context* context, void* auxiliary);
  bool recursivelyMatches(
      ExpressionTestAuxiliary test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr) const;

  bool deepIsOfType(std::initializer_list<ExpressionNode::Type> types,
                    Context* context = nullptr) const;
  bool deepIsMatrix(Context* context = nullptr, bool canContainMatrices = true,
                    bool isReduced = true) const;
  bool deepIsList(Context* context) const;

  // Set of ExpressionTest that can be used with recursivelyMatches
  static bool IsUninitialized(const JuniorExpression e) {
    return e.isUninitialized();
  }
  static bool IsUndefined(const JuniorExpression e) { return e.isUndefined(); }
  static bool IsNAry(const JuniorExpression e) {
    return e.isOfType(
        {ExpressionNode::Type::Addition, ExpressionNode::Type::Multiplication});
  }
  static bool IsApproximate(const JuniorExpression e) {
    return e.isOfType({ExpressionNode::Type::Decimal,
                       ExpressionNode::Type::Float,
                       ExpressionNode::Type::Double});
  }
  static bool IsRandom(const JuniorExpression e) {
    assert(false);
    return false;
  }
  static bool IsMatrix(const JuniorExpression e, Context* context) {
    assert(false);
    return false;
  }
  static bool IsInfinity(const JuniorExpression e) {
    return e.isOfType({ExpressionNode::Type::Infinity});
  }
  static bool IsPercent(const JuniorExpression e) {
    return e.isOfType({ExpressionNode::Type::PercentSimple,
                       ExpressionNode::Type::PercentAddition});
  }
  static bool IsDiscontinuous(const JuniorExpression e, Context* context);
  static bool IsSymbolic(const JuniorExpression e) {
    assert(false);
    return false;
  }
  static bool IsPoint(const JuniorExpression e) {
    return e.isUndefined() || e.type() == ExpressionNode::Type::Point;
  }
  static bool IsSequence(const JuniorExpression e) {
    return e.type() == ExpressionNode::Type::Sequence;
  }
  static bool IsFactorial(const JuniorExpression e) {
    return e.type() == ExpressionNode::Type::Factorial;
  }

  typedef bool (*PatternTest)(const JuniorExpression& e, Context* context,
                              const char* symbol);
  static bool IsRationalFraction(const JuniorExpression& e, Context* context,
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
  void addChildAtIndexInPlace(JuniorExpression t, int index,
                              int currentNumberOfChildren);
  JuniorExpression matrixChild(int i, int j);
  // rank returns -1 if the rank cannot be computed
  int rank(Context* context, bool forceCanonization = false);
  /* Inverse the array in-place. Array has to be given in the form
   * array[row_index][column_index] */
  template <typename T>
  static int ArrayInverse(T* array, int numberOfRows, int numberOfColumns);
};

class Point final : public JuniorExpression {
 public:
  static Point Builder(JuniorExpression x, JuniorExpression y);
  template <typename T>
  Coordinate2D<T> approximate2D(
      const ApproximationContext& approximationContext);
  Layout create2DLayout(Preferences::PrintFloatMode floatDisplayMode,
                        int significantDigits, Context* context) const;
};

// TODO_PCJ: Actually implement methods. Assert its block type is List
class List final : public JuniorExpression {
 public:
  static List Builder();
  bool isListOfPoints(Context* context) const {
    assert(false);
    return false;
  }
  template <typename T>
  JuniorExpression approximateAndRemoveUndefAndSort(
      const ApproximationContext& approximationContext) const {
    assert(false);
    return JuniorExpression();
  }
  void removeChildAtIndexInPlace(int i) {
    assert(false);
    return;
  }
  void addChildAtIndexInPlace(JuniorExpression t, int index,
                              int currentNumberOfChildren);
};

class Boolean final : public JuniorExpression {
 public:
  bool value() const;
};

class Unit final {
 public:
  // Build default unit for given angleUnit preference.
  static JuniorExpression Builder(Preferences::AngleUnit angleUnit);
  static bool IsPureAngleUnit(JuniorExpression expression, bool radianOnly);
  static bool HasAngleDimension(JuniorExpression expression);
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
