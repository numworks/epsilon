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
};

class Superior final : public ExpressionTwoChildren<Superior, SuperiorNode, ComparisonOperator> {
public:
  using ExpressionBuilder::ExpressionBuilder;
};

}

#endif
