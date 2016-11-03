#ifndef CALCULATION_EVALUATECONTEXT_H
#define CALCULATION_EVALUATECONTEXT_H

#include <poincare.h>
#include "calculation_store.h"

namespace Calculation {

class EvaluateContext : public ::Context {
  public:
    EvaluateContext(Context * parentContext, CalculationStore * calculationStore);
    Float * ansValue();
    const Expression * expressionForSymbol(const Symbol * symbol) override;
  private:
    Float m_ansValue;
    CalculationStore * m_calculationStore;
    ::Context * m_context;
};

}

#endif