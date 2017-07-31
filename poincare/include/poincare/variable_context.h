#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/complex.h>

namespace Poincare {

class VariableContext : public Context {
public:
  VariableContext(char name, Context * parentContext = nullptr);
  void setExpressionForSymbolName(Evaluation * expression, const Symbol * symbol) override;
  const Evaluation * expressionForSymbol(const Symbol * symbol) override;
private:
  char m_name;
  Complex m_value;
  Context * m_parentContext;
};

}

#endif
