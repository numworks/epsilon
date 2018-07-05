#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/evaluation.h>

namespace Poincare {

class VariableContext : public Context {
public:
  VariableContext(char name, Context * parentContext = nullptr);
  ~VariableContext();
  /* This function could be avoid by doing:
   * {
   *   Expression * e = Expression::CreateDecimal<T>(value);
   *   variableContext.setExpressionForSymbolName(e, &xSymbol, variableContext);
   *   delete e;
   * }
   * But this would imply to allocated twice the expression (as e is cloned in
   * setExpressionForSymbolName).
   */
  template<typename T> void setApproximationForVariable(T value);
  void setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) override;
  const Expression * expressionForSymbol(const Symbol * symbol) override;
private:
  char m_name;
  Expression * m_value;
  Context * m_parentContext;
};

}

#endif
