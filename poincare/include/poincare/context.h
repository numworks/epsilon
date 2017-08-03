#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <poincare/evaluation.h>
#include <poincare/symbol.h>

namespace Poincare {

class Context {
public:
  virtual const Evaluation * expressionForSymbol(const Symbol * symbol) = 0;
  virtual void setExpressionForSymbolName(Evaluation * expression, const Symbol * symbol) = 0;
};

}

#endif
