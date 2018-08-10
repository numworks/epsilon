#ifndef POINCARE_ALLOCATION_FAILURE_EVALUATION_H
#define POINCARE_ALLOCATION_FAILURE_EVALUATION_H

#include <poincare/evaluation.h>
#include <poincare/complex.h>
#include <poincare/undefined.h>

namespace Poincare {

template <template<typename> class T, class U>
class AllocationFailureEvaluationNode : public T<U> {
public:
  // EvaluationNode
  typename EvaluationNode<U>::Type type() const override { return EvaluationNode<U>::Type::AllocationFailure; }
  bool isUndefined() const override { return true; }
  U toScalar() const override { return NAN; }
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override { return Undefined(); }
  std::complex<U> trace() const override { return std::complex<U>(NAN); }
  std::complex<U> determinant() const override { return std::complex<U>(NAN); }
  // TreeNode
  size_t size() const override { return sizeof(AllocationFailureEvaluationNode<T, U>); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailureEvaluation";  }
#endif
  bool isAllocationFailure() const override { return true; }
};

}

#endif
