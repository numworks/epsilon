#ifndef POINCARE_ALLOCATION_FAILED_EVALUATION_H
#define POINCARE_ALLOCATION_FAILED_EVALUATION_H

#include <poincare/evaluation.h>
#include <poincare/complex.h>

namespace Poincare {

template<typename T>
class AllocationFailedEvaluationNode : public EvaluationNode<T> {
public:
  // EvaluationNode
  typename EvaluationNode<T>::Type type() const override { return EvaluationNode<T>::Type::AllocationFailure; }
  bool isUndefined() const override { return true; }
  T toScalar() const override{ return NAN; }
  ExpressionReference complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
  std::complex<T> trace() const override { return std::complex<T>(NAN); }
  std::complex<T> determinant() const override { return std::complex<T>(NAN); }
  EvaluationReference<T> inverse() const override { return ComplexReference<T>::Undefined(); }
  virtual EvaluationReference<T> transpose() const override { return ComplexReference<T>::Undefined(); }
  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedEvaluationNode); }
  const char * description() const override { return "Allocation Failed";  }
  bool isAllocationFailure() const override { return true; }
};

template<typename T>
class AllocationFailedEvaluationReference : public EvaluationReference<T> {
public:
  AllocationFailedEvaluationReference() : EvaluationReference<T>() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<AllocationFailedEvaluationNode<T>>();
    this->m_identifier = node->identifier();
  }
  AllocationFailedEvaluationReference(TreeNode * n) : EvaluationReference<T>(n) {}
};

}

#endif
