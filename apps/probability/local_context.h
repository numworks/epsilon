#ifndef PROBABILITY_LOCAL_CONTEXT_H
#define PROBABILITY_LOCAL_CONTEXT_H

#include <poincare.h>

namespace Probability {

class LocalContext : public Context {
public:
  LocalContext(Context * parentContext);
  void setExpressionForSymbolName(Expression * expression, const Symbol * symbol) override;
  const Expression * expressionForSymbol(const Symbol * symbol) override;
private:
  Float m_tValue;
  Float m_aValue;
  Float m_bValue;
  Context * m_parentContext;
};

}

#endif