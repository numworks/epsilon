#ifndef POINCARE_SUPERIOR_EQUAL_H
#define POINCARE_SUPERIOR_EQUAL_H

#include <poincare/comparison_operator.h>

namespace Poincare {

class SuperiorEqualNode final : public ComparisonOperatorNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(SuperiorEqualNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "SuperiorEqual";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::SuperiorEqual; }
private:
  // ComparisonOperatorNode
  CodePoint comparisonCodePoint() const override { return UCodePointSuperiorEqual; };
  const char * comparisonString() const override { return "≥"; };
};

class SuperiorEqual final : public HandleTwoChildrenWithParent<SuperiorEqual, SuperiorEqualNode, ComparisonOperator> {
public:
  using Handle::Handle, Handle::Builder;
};

}

#endif
