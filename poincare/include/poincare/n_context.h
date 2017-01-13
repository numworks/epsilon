#ifndef POINCARE_N_CONTEXT_H
#define POINCARE_N_CONTEXT_H

#include <poincare/context.h>
#include <poincare/float.h>

class NContext : public Context {
public:
  NContext(Context * parentContext = nullptr);
  void setExpressionForSymbolName(Expression * expression, const Symbol * symbol) override;
  const Expression * expressionForSymbol(const Symbol * symbol) override;
private:
  Float m_nValue;
  Context * m_parentContext;
};

#endif