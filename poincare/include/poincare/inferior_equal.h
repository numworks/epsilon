#ifndef POINCARE_INFERIOR_EQUAL_H
#define POINCARE_INFERIOR_EQUAL_H

#include <poincare/comparison_operator.h>

namespace Poincare {

class InferiorEqualNode final : public ComparisonOperatorNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(InferiorEqualNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "InferiorEqual";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::InferiorEqual; }
private:
  // ComparisonOperatorNode
  CodePoint comparisonCodePoint() const override { return UCodePointInferiorEqual; };
  const char * comparisonString() const override { return "≤"; };
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
};

class InferiorEqual final : public ComparisonOperator {
public:
  InferiorEqual(const InferiorEqualNode * n) : ComparisonOperator(n) {}
  static InferiorEqual Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<InferiorEqual, InferiorEqualNode>({child0, child1}); }

  // Expression
  Expression shallowReduce();
};

}

#endif
