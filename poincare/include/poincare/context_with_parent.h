#ifndef POINCARE_CONTEXT_WITH_PARENT_H
#define POINCARE_CONTEXT_WITH_PARENT_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <assert.h>

namespace Poincare {

class ContextWithParent : public Context {
public:
  ContextWithParent(Context * parentContext) : m_parentContext(parentContext) { assert(parentContext); }

  // Context
  virtual SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) override { return m_parentContext->expressionTypeForIdentifier(identifier, length); }
  bool setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) override { return m_parentContext->setExpressionForSymbolAbstract(expression, symbol); }
  const Expression expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) override { return m_parentContext->expressionForSymbolAbstract(symbol, clone, unknownSymbolValue); }

private:
  Context * m_parentContext;
};

}

#endif
