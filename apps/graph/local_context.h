#ifndef GRAPH_LOCAL_CONTEXT_H
#define GRAPH_LOCAL_CONTEXT_H

#include <poincare.h>

namespace Graph {

class LocalContext : public Context {
public:
  LocalContext(Context * parentContext);
  void setExpressionForSymbolName(Expression * expression, const Symbol * symbol) override;
  const Expression * expressionForSymbol(const Symbol * symbol) override;
private:
  Float m_xValue;
  Context * m_parentContext;
};

}

#endif