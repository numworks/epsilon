#ifndef POINCARE_ALLOCATION_FAILED_EXPRESSION_NODE_H
#define POINCARE_ALLOCATION_FAILED_EXPRESSION_NODE_H

#include <poincare/ghost_expression_node.h>
#include <poincare/allocation_failed_layout_node.h>

namespace Poincare {

class AllocationFailedExpressionNode : public GhostExpressionNode {
public:
  // ExpressionNode
  Type type() const override { return Type::AllocationFailure; }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return AllocationFailedLayoutRef(); }

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedExpressionNode); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailedExpression";  }
#endif
  bool isAllocationFailure() const override { return true; }
};

class AllocationFailedExpressionRef : public ExpressionReference {
public:
  AllocationFailedExpressionRef() : ExpressionReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<AllocationFailedExpressionNode>();
    m_identifier = node->identifier();
  }
  AllocationFailedExpressionRef(TreeNode * n) : ExpressionReference(n) {}
};

}

#endif
