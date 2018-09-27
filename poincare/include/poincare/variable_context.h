#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/float.h>

namespace Poincare {

class VariableContext : public Context {
public:
  VariableContext(const char * name, Context * parentContext = nullptr);
  template<typename T>
  void setApproximationForVariable(T value);

  // Context
  void setExpressionForSymbolName(const Expression & expression, const char * symbolName, Context & context) override;
  const Expression expressionForSymbol(const Symbol & symbol) override;

private:
  const char * m_name;
  Expression m_value;
  Context * m_parentContext;
};

}

#endif
