#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <assert.h>
extern "C" {
#include <math.h>
}

namespace Poincare {

template<typename T>
VariableContext<T>::VariableContext(char name, Context * parentContext) :
  m_name(name),
  m_value(Complex<T>::Float(NAN)),
  m_parentContext(parentContext)
{
}

template<typename T>
void VariableContext<T>::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() == m_name) {
    if (expression == nullptr) {
      return;
    }
    Evaluation<T> * evaluation = expression->evaluate<T>(*m_parentContext);
    /* WARNING: We assume that the evaluation of expression is a real */
    m_value = Complex<T>::Float(evaluation->toScalar());
    delete evaluation;
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

template<typename T>
const Expression * VariableContext<T>::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == m_name) {
    return &m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

template class Poincare::VariableContext<float>;
template class Poincare::VariableContext<double>;

}
