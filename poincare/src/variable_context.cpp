#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <poincare/undefined.h>
#include <poincare/expression.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

VariableContext::VariableContext(char name, Context * parentContext) :
  m_name(name),
  m_value(nullptr),
  m_parentContext(parentContext)
{
}

VariableContext::~VariableContext() {
  if (m_value) {
    delete m_value;
    m_value = nullptr;
  }
}

template<typename T> void VariableContext::setApproximationForVariable(T value) {
  if (m_value) {
    delete m_value;
    m_value = nullptr;
  }
  m_value = Expression::CreateDecimal<T>(value);
}

void VariableContext::setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) {
  if (symbol->name() == m_name) {
    if (expression == nullptr) {
      return;
    }
    if (m_value) {
      delete m_value;
      m_value = nullptr;
    }
    assert(false); // TODO: remove
    m_value = expression->clone();
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol, context);
  }
}

const Expression * VariableContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == m_name) {
    return m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

template void VariableContext::setApproximationForVariable<double>(double);
template void VariableContext::setApproximationForVariable<float>(float);
}
