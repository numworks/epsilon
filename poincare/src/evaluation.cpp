#include <poincare/evaluation.h>
#include <poincare/expression.h>
#include <poincare/uninitialized_evaluation_node.h>

namespace Poincare {

template<typename T>
Evaluation<T>::Evaluation() : Evaluation<T>(UninitializedEvaluationNode<T>::UninitializedEvaluationStaticNode()) {}

template<typename T>
Expression Evaluation<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  return node()->complexToExpression(complexFormat);
}

template Evaluation<float>::Evaluation();
template Evaluation<double>::Evaluation();
template Expression Evaluation<float>::complexToExpression(Preferences::ComplexFormat) const;
template Expression Evaluation<double>::complexToExpression(Preferences::ComplexFormat) const;

}
