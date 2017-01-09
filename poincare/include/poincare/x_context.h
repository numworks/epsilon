#ifndef POINCARE_LOCAL_CONTEXT_H
#define POINCARE_LOCAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/float.h>

class XContext : public Context {
public:
  XContext(Context * parentContext = nullptr);
  void setExpressionForSymbolName(Expression * expression, const Symbol * symbol) override;
  const Expression * expressionForSymbol(const Symbol * symbol) override;
private:
  Float m_xValue;
  Context * m_parentContext;
};

#endif