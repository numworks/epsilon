#include "evaluate_context.h"
#include <string.h>

Graph::EvaluateContext::EvaluateContext(::Context * parentContext) :
  m_xValue(Float(0.0f)),
  m_context(parentContext)
{
}

void Graph::EvaluateContext::setOverridenValueForSymbolX(float f) {
  m_xValue = Float(f);
}

const Expression * Graph::EvaluateContext::operator[](const char * symbolName) const {
  if (strcmp("x", symbolName) == 0) {
    return &m_xValue;
  } else {
    Context context = *m_context;
    return context[symbolName];
  }
}
