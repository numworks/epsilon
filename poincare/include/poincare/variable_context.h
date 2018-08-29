#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/float.h>

namespace Poincare {

template<typename T>
class VariableContext : public Context {
public:
  VariableContext(char name, Context * parentContext = nullptr);
  void setApproximationForVariable(T value);

  // Context
  void setExpressionForSymbolName(const Expression & expression, const Symbol & symbol, Context & context) override;
  const Expression expressionForSymbol(const Symbol & symbol) override;

private:
  char m_name;
  Float<T> m_value;
  Context * m_parentContext;
};

}

#endif
