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
  void setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) override;
  const Expression expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) override;

private:
  const char * m_name;
  Expression m_value;
  Context * m_parentContext;
};

}

#endif
