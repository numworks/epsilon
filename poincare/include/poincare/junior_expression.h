#ifndef POINCARE_JUNIOR_EXPRESSION_H
#define POINCARE_JUNIOR_EXPRESSION_H

#include <poincare/expression.h>
#include <poincare/symbol.h>
#include <poincare_junior/src/memory/tree.h>

namespace Poincare {

class JuniorExpressionNode final : public ExpressionNode {
  friend class JuniorExpression;

 public:
  JuniorExpressionNode(const PoincareJ::Tree* tree, size_t treeSize) {
    memcpy(m_blocks, tree, treeSize);
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
  OLayout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                       int numberOfSignificantDigits, Context* context) const {
    // TODO PCJ
    assert(false);
    return Layout();
  }
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override {
    // TODO PCJ
    assert(false);
    return 0;
  }

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override {
    assert(false);
    return Expression();
  }
  LayoutShape leftLayoutShape() const override {
    // TODO PCJ: Remove
    assert(false);
    return LayoutShape::BoundaryPunctuation;
  }

 private:
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override {
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

class JuniorExpression final : public Expression {
  friend class JuniorExpressionNode;

 public:
  JuniorExpression() {}
  JuniorExpression(const Expression& other) { *this = other; }

  static JuniorExpression Builder(const PoincareJ::Tree* tree);
  // Eat the tree
  static JuniorExpression Builder(PoincareJ::Tree* tree);
  static JuniorExpression Juniorize(Expression e);
  PoincareJ::Tree* tree() const {
    return const_cast<JuniorExpression*>(this)->node()->tree();
  }

  JuniorExpression operator=(Expression&& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorExpression operator=(const Expression& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorExpressionNode* node() const {
    return static_cast<JuniorExpressionNode*>(Expression::node());
  }

  Expression shallowReduce(ReductionContext reductionContext) {
    // TODO PCJ
    assert(false);
    return Expression();
  }
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) {
    // TODO PCJ: Remove
    assert(false);
    return false;
  }
};

}  // namespace Poincare

#endif
