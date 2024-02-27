#ifndef POINCARE_JUNIOR_EXPRESSION_H
#define POINCARE_JUNIOR_EXPRESSION_H

#include <poincare/old_expression.h>
#include <poincare/symbol.h>
#include <poincare_junior/src/memory/tree.h>

namespace Poincare {

class JuniorExpressionNode final : public ExpressionNode {
  friend class JuniorExpression;

 public:
  JuniorExpressionNode(const PoincareJ::Tree* tree, size_t treeSize) {
    memcpy(m_blocks, tree->block(), treeSize);
  }

  // TreeNode
  size_t size() const override {
    return sizeof(JuniorExpressionNode) + tree()->treeSize();
  }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "JuniorExpression";
  }
  void logAttributes(std::ostream& stream) const override {
    tree()->log(stream);
  }
#endif

  // Properties
  Type type() const override { return Type::JuniorExpression; }
  // TODO PCJ: Plug in approximation
  // TrinaryBoolean isPositive(Context* context) const override;
  // TrinaryBoolean isNull(Context* context) const override;

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
      const ApproximationContext& approximationContext) const override {
    // TODO PCJ: Plug in approximation
    assert(false);
    return Evaluation<float>();
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    // TODO PCJ: Plug in approximation
    assert(false);
    return Evaluation<double>();
  }

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
                OExpression symbolValue) override {
    // TODO PCJ: Remove
    assert(false);
    return false;
  }

  // PCJ
  const PoincareJ::Tree* tree() const {
    return PoincareJ::Tree::FromBlocks(m_blocks);
  }
  PoincareJ::Tree* tree() { return PoincareJ::Tree::FromBlocks(m_blocks); }
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

  static JuniorExpression Builder(const PoincareJ::Tree* tree);
  // Eat the tree
  static JuniorExpression Builder(PoincareJ::Tree* tree);
  static JuniorExpression Juniorize(OExpression e);
  PoincareJ::Tree* tree() const {
    return const_cast<JuniorExpression*>(this)->node()->tree();
  }
  JuniorExpression childAtIndex(int i) const;

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

  OExpression shallowReduce(ReductionContext reductionContext) {
    // TODO PCJ
    assert(false);
    return OExpression();
  }
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) {
    // TODO PCJ: Remove
    assert(false);
    return false;
  }

#if 1  // TODO_PCJ
  typedef TrinaryBoolean (*ExpressionTrinaryTest)(const JuniorExpression e,
                                                  Context* context,
                                                  void* auxiliary);
  struct IgnoredSymbols {
    Symbol* head;
    void* tail;
  };
  bool recursivelyMatches(
      ExpressionTrinaryTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr,
      IgnoredSymbols* ignoredSymbols = nullptr) const {
    return false;
  }
  typedef bool (*ExpressionTest)(const JuniorExpression e, Context* context);
  bool recursivelyMatches(
      ExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const {
    return false;
  }
  typedef bool (*SimpleExpressionTest)(const JuniorExpression e);
  bool recursivelyMatches(
      SimpleExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const {
    return false;
  }
  typedef bool (*ExpressionTestAuxiliary)(const JuniorExpression e,
                                          Context* context, void* auxiliary);
  bool recursivelyMatches(
      ExpressionTestAuxiliary test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr) const {
    return false;
  }
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
  static bool IsRandom(const JuniorExpression e) { return false; }
  static bool IsMatrix(const JuniorExpression e, Context* context) {
    return false;
  }
  static bool IsInfinity(const JuniorExpression e) {
    return e.isOfType({ExpressionNode::Type::Infinity});
  }
  static bool IsPercent(const JuniorExpression e) {
    return e.isOfType({ExpressionNode::Type::PercentSimple,
                       ExpressionNode::Type::PercentAddition});
  }
  static bool IsDiscontinuous(const JuniorExpression e, Context* context) {
    return false;
  }
  static bool IsSymbolic(const JuniorExpression e) { return false; }
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
    return false;
  }
  bool isLinearCombinationOfFunction(Context* context, PatternTest testFunction,
                                     const char* symbol) const {
    return false;
  }
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

}  // namespace Poincare

#endif
