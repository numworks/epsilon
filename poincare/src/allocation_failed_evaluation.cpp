#include <poincare/allocation_failed_evaluation.h>
#include <poincare/expression.h>
#include <poincare/undefined.h>

namespace Poincare {

template<typename T>
Expression AllocationFailedEvaluationNode<T>::complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const {
  return Undefined();
}

}
