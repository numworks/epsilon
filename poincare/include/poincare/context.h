#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

namespace Poincare {

class Expression;
class SymbolAbstract;
class Symbol; //TODO remove?

class Context {
public:
  virtual const Expression expressionForSymbol(const SymbolAbstract & symbol) = 0;
  virtual void setExpressionForSymbol(const Expression & expression, const Symbol & symbol, Context & context) = 0;
};

}

#endif
