#ifndef POINCARE_EQUAL_H
#define POINCARE_EQUAL_H

#include <poincare/comparison_operator.h>

namespace Poincare {

class EqualNode final : public ComparisonOperatorNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(EqualNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Equal";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::Equal; }
private:
  // ComparisonOperatorNode
  CodePoint comparisonCodePoint() const override { return '='; };
  const char * comparisonString() const override { return "="; };
};

class Equal final : public ComparisonOperator {
public:
  Equal(const EqualNode * n) : ComparisonOperator(n) {}
  static Equal Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Equal, EqualNode>({child0, child1}); }
};

}

#endif
