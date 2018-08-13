#ifndef POINCARE_ALLOCATION_FAILURE_NODE_H
#define POINCARE_ALLOCATION_FAILURE_NODE_H

#include <stddef.h>
#include <poincare/exception_node.h>

namespace Poincare {

template <typename T>
class AllocationFailureNode : public ExceptionNode<T> {
public:

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailureNode<T>); }
  bool isException() const override { return false; } //TODO ?
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
