#ifndef CALCULATION_LOCAL_CONTEXT_H
#define CALCULATION_LOCAL_CONTEXT_H

#include <poincare.h>
#include "calculation_store.h"

namespace Calculation {

class LocalContext : public Context {
public:
  LocalContext(GlobalContext * parentContext, CalculationStore * calculationStore);
  void setExpressionForSymbolName(Expression * expression, const Symbol * symbol) override;
  const Expression * expressionForSymbol(const Symbol * symbol) override;
private:
  Expression * ansValue();
  CalculationStore * m_calculationStore;
  GlobalContext * m_parentContext;
};

}

#endif