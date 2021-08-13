#ifndef POINCARE_INEQUAL_H
#define POINCARE_INEQUAL_H

#include <poincare/comparison_operator.h>

namespace Poincare {

class InequalNode final : public ComparisonOperatorNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(InequalNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Inequal";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::Inequal; }
private:
  // ComparisonOperatorNode
  CodePoint comparisonCodePoint() const override { return UCodePointInequal; };
  const char * comparisonString() const override { return "≠"; };
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
};

class Inequal final : public ComparisonOperator {
public:
  Inequal(const InequalNode * n) : ComparisonOperator(n) {}
  static Inequal Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Inequal, InequalNode>({child0, child1}); }

  // Expression
  Expression shallowReduce();
};

}

#endif
