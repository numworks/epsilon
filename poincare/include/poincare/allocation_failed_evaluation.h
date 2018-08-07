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
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
  std::complex<T> trace() const override { return std::complex<T>(NAN); }
  std::complex<T> determinant() const override { return std::complex<T>(NAN); }
  Evaluation<T> inverse() const override { return Complex<T>::Undefined(); }
  virtual Evaluation<T> transpose() const override { return Complex<T>::Undefined(); }
  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedEvaluationNode); }
#if TREE_LOG
  const char * description() const override { return "Allocation Failed";  }
#endif
  bool isAllocationFailure() const override { return true; }
};

}

#endif
