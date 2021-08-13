#ifndef POINCARE_SUPERIOR_H
#define POINCARE_SUPERIOR_H

#include <poincare/comparison_operator.h>

namespace Poincare {

class SuperiorNode final : public ComparisonOperatorNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(SuperiorNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Superior";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::Superior; }
private:
  // ComparisonOperatorNode
  CodePoint comparisonCodePoint() const override { return '>'; };
  const char * comparisonString() const override { return ">"; };
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
};

class Superior final : public ComparisonOperator {
public:
  Superior(const SuperiorNode * n) : ComparisonOperator(n) {}
  static Superior Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Superior, SuperiorNode>({child0, child1}); }

  // Expression
  Expression shallowReduce();
};

}

#endif
