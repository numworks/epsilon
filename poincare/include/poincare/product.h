#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/sequence.h>

namespace Poincare {

class ProductNode final : public SequenceNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(ProductNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Product";
  }
#endif

  Type type() const override { return Type::Product; }
  Expression replaceUnknown(const Symbol & symbol) override;

private:
  float emptySequenceValue() const override { return 1.0f; }
  Layout createSequenceLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Evaluation<double> evaluateWithNextTerm(DoublePrecision p, Evaluation<double> a, Evaluation<double> b) const override {
    return templatedApproximateWithNextTerm<double>(a, b);
  }
  Evaluation<float> evaluateWithNextTerm(SinglePrecision p, Evaluation<float> a, Evaluation<float> b) const override {
    return templatedApproximateWithNextTerm<float>(a, b);
  }
  template<typename T> Evaluation<T> templatedApproximateWithNextTerm(Evaluation<T> a, Evaluation<T> b) const;
};

class Product final : public Expression {
friend class ProductNode;
public:
  Product(const ProductNode * n) : Expression(n) {}
  static Product Builder(Expression child0, Symbol child1, Expression child2, Expression child3) { return Product(child0, child1, child2, child3); }
  static Expression UntypedBuilder(Expression children) {
    if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
      // Second parameter must be a Symbol.
      return Expression();
    }
    return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2), children.childAtIndex(3));
  }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("product", 4, &UntypedBuilder);
private:
  Product(Expression child0, Expression child1, Expression child2, Expression child3) : Expression(TreePool::sharedPool()->createTreeNode<ProductNode>()) {
    assert(child1.type() == ExpressionNode::Type::Symbol);
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
    replaceChildAtIndexInPlace(2, child2);
    replaceChildAtIndexInPlace(3, child3);
  }
};

}

#endif
