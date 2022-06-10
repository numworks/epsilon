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

class Inferior final : public ExpressionTwoChildren<Inferior, InferiorNode, ComparisonOperator> {
public:
  using ExpressionBuilder::ExpressionBuilder;
};

}

#endif
