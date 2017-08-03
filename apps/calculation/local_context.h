#ifndef CALCULATION_LOCAL_CONTEXT_H
#define CALCULATION_LOCAL_CONTEXT_H

#include <poincare.h>
#include "calculation_store.h"

namespace Calculation {

class LocalContext : public Poincare::Context {
public:
  LocalContext(Poincare::GlobalContext * parentContext, CalculationStore * calculationStore);
  void setExpressionForSymbolName(Poincare::Expression * expression, const Poincare::Symbol * symbol) override;
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
private:
  Poincare::Evaluation<double> * ansValue();
  CalculationStore * m_calculationStore;
  Poincare::GlobalContext * m_parentContext;
};

}

#endif
