#ifndef POINCARE_INFERIOR_H
#define POINCARE_INFERIOR_H

#include <poincare/comparison_operator.h>

namespace Poincare {

class InferiorNode final : public ComparisonOperatorNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(InferiorNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Inferior";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::Inferior; }
private:
  // ComparisonOperatorNode
  CodePoint comparisonCodePoint() const override { return '<'; };
  const char * comparisonString() const override { return "<"; };
};

class Inferior final : public ComparisonOperator {
public:
  Inferior(const InferiorNode * n) : ComparisonOperator(n) {}
  static Inferior Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Inferior, InferiorNode>({child0, child1}); }
};

}

#endif
