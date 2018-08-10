#ifndef POINCARE_ALLOCATION_FAILURE_NODE_H
#define POINCARE_ALLOCATION_FAILURE_NODE_H

#include <stddef.h>

namespace Poincare {

template <typename T>
class AllocationFailureNode : public T {
public:
  AllocationFailureNode() : T() {}
  AllocationFailureNode(T node) : T(node) {}

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailureNode<T>); }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AllocationFailure[";
    T::logNodeName(stream);
    stream << "]";
  }
#endif
};

}

#endif
