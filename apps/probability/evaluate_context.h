#ifndef PROBABILITY_EVALUATECONTEXT_H
#define PROBABILITY_EVALUATECONTEXT_H

#include <poincare.h>

namespace Probability {

class EvaluateContext : public ::Context {
  public:
    EvaluateContext(Context * parentContext);
    void setOverridenValueForSymbolT(float f);
    void setOverridenValueForFirstParameter(float f);
    void setOverridenValueForSecondParameter(float f);
    const Expression * expressionForSymbol(const Symbol * symbol) override;
  private:
    Float m_tValue;
    Float m_firstParameterValue;
    Float m_secondParameterValue;
    ::Context * m_context;
};

}

#endif