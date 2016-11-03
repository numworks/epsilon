#ifndef GRAPH_EVALUATECONTEXT_H
#define GRAPH_EVALUATECONTEXT_H

#include <poincare.h>

namespace Graph {

class EvaluateContext : public ::Context {
  public:
    EvaluateContext(Context * parentContext);
    void setOverridenValueForSymbolX(float f);
    const Expression * expressionForSymbol(const Symbol * symbol) override;
  private:
    Float m_xValue;
    ::Context * m_context;
};

}

#endif