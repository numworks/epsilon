#ifndef POINCARE_UNINITIALIZED_EVALUATION_NODE_H
#define POINCARE_UNINITIALIZED_EVALUATION_NODE_H

#include <poincare/evaluation.h>
#include <poincare/exception_evaluation_node.h>
#include <stdio.h>

namespace Poincare {

/* All UninitializedExpressions should be caught so its node methods are
 * asserted false. */

template<typename T>
class UninitializedEvaluationNode : public ExceptionEvaluationNode<EvaluationNode, T> {
public:
  static UninitializedEvaluationNode<T> * UninitializedEvaluationStaticNode();
  // TreeNode
  bool isUninitialized() const override { return true; }
  size_t size() const override { return sizeof(UninitializedEvaluationNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedEvaluation";
  }
#endif
};

}

#endif
