#ifndef POINCARE_STORE_H
#define POINCARE_STORE_H

#include <poincare/expression.h>
#include <poincare/symbol_abstract.h>
#include <poincare/evaluation.h>

namespace Poincare {

class StoreNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(StoreNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Store";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::Store; }
  int polynomialDegree(Context * context, const char * symbolName) const override { return -1; }

private:
  // Simplification
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext) override;
  Expression shallowReduce(ReductionContext reductionContext) override;
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evalutation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Store final : public Expression {
friend class StoreNode;
public:
  Store(const StoreNode * n) : Expression(n) {}
  static Store Builder(Expression value, SymbolAbstract symbol) { return TreeHandle::FixedArityBuilder<Store, StoreNode>(ArrayBuilder<TreeHandle>(value, symbol).array(), 2); }

  // Store
  const SymbolAbstract symbol() const {
    assert(childAtIndex(1).type() == ExpressionNode::Type::Symbol
        || childAtIndex(1).type() == ExpressionNode::Type::Function);
    return childAtIndex(1).convert<const SymbolAbstract>();
  }
  const Expression value() const {
    return childAtIndex(0);
  }

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

private:
  Expression storeValueForSymbol(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  StoreNode * node() const { return static_cast<StoreNode *>(Expression::node()); }
};

}

#endif
