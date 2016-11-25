#ifndef CALCULATION_EVALUATECONTEXT_H
#define CALCULATION_EVALUATECONTEXT_H

#include <poincare.h>
#include "calculation_store.h"

namespace Calculation {

class EvaluateContext : public ::Context {
  public:
    EvaluateContext(Context * parentContext, CalculationStore * calculationStore);
    Expression * ansValue();
    const Expression * expressionForSymbol(const Symbol * symbol) override;
  private:
    Expression * m_ansValue;
    CalculationStore * m_calculationStore;
    ::Context * m_context;
};

}

#endif