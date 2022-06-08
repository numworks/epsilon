#ifndef POINCARE_EMPTY_CONTEXT_H
#define POINCARE_EMPTY_CONTEXT_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <assert.h>

namespace Poincare {

class EmptyContext : public Context {
public:
  // Context
  SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) override { return SymbolAbstractType::None; }
  bool setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) override { assert(false); return false; }
  const Expression expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) override { return Expression(); }
};

}

#endif
