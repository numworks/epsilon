#ifndef POINCARE_ALLOCATION_FAILURE_EXPRESSION_NODE_H
#define POINCARE_ALLOCATION_FAILURE_EXPRESSION_NODE_H

#include <poincare/exception_expression_node.h>

namespace Poincare {

template <typename T>
class AllocationFailureExpressionNode : public ExceptionExpressionNode<T> {
public:
  // ExpressionNode
  ExpressionNode::Type type() const override { return ExpressionNode::Type::AllocationFailure; }
  // TreeNode
  void incrementNumberOfChildren(int increment = 1) override {}
  void decrementNumberOfChildren(int decrement = 1) override {}
  size_t size() const override { return sizeof(AllocationFailureExpressionNode<T>); }
  bool isAllocationFailure() const override { return true; }
  TreeNode * uninitializedStaticNode() const override { assert(false); return nullptr; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AllocationFailureExpression[";
    T::logNodeName(stream);
    stream << "]";
  }
#endif
};

}

#endif
