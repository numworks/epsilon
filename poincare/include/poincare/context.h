#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

namespace Poincare {

class ExpressionReference;
class SymbolReference;

class Context {
public:
  virtual const ExpressionReference expressionForSymbol(const SymbolReference symbol) = 0;
  virtual void setExpressionForSymbolName(const ExpressionReference expression, const SymbolReference symbol, Context & context) = 0;
};

}

#endif
