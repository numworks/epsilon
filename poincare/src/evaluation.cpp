#include <poincare/evaluation.h>
#include <poincare/expression.h>

namespace Poincare {

template<typename T>
Expression Evaluation<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  return node()->complexToExpression(complexFormat);
}

template Expression Evaluation<float>::complexToExpression(Preferences::ComplexFormat) const;
template Expression Evaluation<double>::complexToExpression(Preferences::ComplexFormat) const;

}
