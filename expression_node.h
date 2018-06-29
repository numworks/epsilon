#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "tree_node.h"

class ExpressionNode : public TreeNode {
public:
   enum class Type : uint8_t {
    AllocationFailure = 0,
    Float = 1,
    Addition,
    SimplificationRoot
   };

  // Expression
  virtual Type type() const = 0;
  virtual float approximate() = 0;

  void deepReduce() {
    assert(parentTree() != nullptr);
    for (int i = 0; i < numberOfChildren(); i++) {
      child(i)->deepReduce();
    }
    shallowReduce();
  }

  virtual bool shallowReduce() {
    for (int i = 0; i < numberOfChildren(); i++) {
      if (child(i)->isAllocationFailure()) {
        replaceWithAllocationFailure();
        return true;
      }
    }
    return false;
  }

  // Allocation failure
  static TreeNode * FailedAllocationStaticNode();
  static int AllocationFailureNodeIdentifier() {
    return FailedAllocationStaticNode()->identifier();
  }
  int allocationFailureNodeIdentifier() override {
    return AllocationFailureNodeIdentifier();
  }
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // Hierarchy
  ExpressionNode * child(int i) { return static_cast<ExpressionNode *>(childTreeAtIndex(i)); }
protected:
  void sortChildren();
};

#endif
