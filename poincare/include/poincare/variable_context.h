#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/complex.h>

namespace Poincare {

template<typename T>
class VariableContext : public Context {
public:
  VariableContext(char name, Context * parentContext = nullptr);
  void setExpressionForSymbolName(Expression * expression, const Symbol * symbol) override;
  const Expression * expressionForSymbol(const Symbol * symbol) override;
private:
  char m_name;
  Complex<T> m_value;
  Context * m_parentContext;
};

}

#endif
