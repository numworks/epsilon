#include <poincare/evaluation.h>
#include <poincare/allocation_failed_evaluation.h>
#include <poincare/expression.h>

namespace Poincare {

template<typename T>
Expression Evaluation<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  return node()->complexToExpression(complexFormat);
}

template Expression EvaluationNode<float>::complexToExpression(Preferences::ComplexFormat) const;
template Expression EvaluationNode<double>::complexToExpression(Preferences::ComplexFormat) const;

template class Evaluation<float>;
template class Evaluation<double>;

}
