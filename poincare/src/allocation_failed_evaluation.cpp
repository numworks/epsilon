#include <poincare/allocation_failed_evaluation.h>
#include <poincare/expression_reference.h>
#include <poincare/undefined.h>

namespace Poincare {

template<typename T>
ExpressionReference AllocationFailedEvaluationNode<T>::complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const {
  return UndefinedReference();
}

}
