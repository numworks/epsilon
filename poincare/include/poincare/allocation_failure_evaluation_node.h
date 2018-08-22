#ifndef POINCARE_ALLOCATION_FAILURE_EVALUATION_NODE_H
#define POINCARE_ALLOCATION_FAILURE_EVALUATION_NODE_H

#include <poincare/exception_evaluation_node.h>
#include <poincare/complex.h>
#include <poincare/undefined.h>

namespace Poincare {

template <template<typename> class T, class U>
class AllocationFailureEvaluationNode : public ExceptionEvaluationNode<T, U> {
public:
  // EvaluationNode
  typename EvaluationNode<U>::Type type() const override { return EvaluationNode<U>::Type::AllocationFailure; }

  // TreeNode
  bool isAllocationFailure() const override { return true; }
  size_t size() const override { return sizeof(AllocationFailureEvaluationNode<T, U>); }
  TreeNode * uninitializedStaticNode() const override { assert(false); return nullptr; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AllocationFailureEvaluation";
    T::logNodeName(stream);
  }
#endif
};

}

#endif
