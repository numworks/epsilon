#ifndef POINCARE_ALLOCATION_FAILURE_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILURE_LAYOUT_NODE_H

#include <poincare/exception_layout_node.h>

namespace Poincare {

template <typename T>
class AllocationFailureLayoutNode : public ExceptionLayoutNode<T> {
public:
  size_t size() const override { return sizeof(AllocationFailureLayoutNode<T>); }
  bool isAllocationFailure() const override { return true; }
  TreeNode * uninitializedStaticNode() const override { assert(false); return nullptr; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AllocationFailureLayout";
    T::logNodeName(stream);
  }
#endif
};

/*
template <typename T>
class AllocationFailureLayoutRef : public LayoutReference {
public:
  AllocationFailedLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<AllocationFailureLayoutNode<T>>()) {}
};
*/

}

#endif
