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
};

class InferiorEqual final : public HandleTwoChildrenWithParent<InferiorEqual, InferiorEqualNode, ComparisonOperator> {
public:
  using Handle::Handle, Handle::Builder;
};

}

#endif
