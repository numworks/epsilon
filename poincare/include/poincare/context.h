#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <poincare/expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class Context {
public:
  virtual const Expression * expressionForSymbol(const Symbol * symbol) = 0;
  virtual void setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) = 0;
};

}

#endif
