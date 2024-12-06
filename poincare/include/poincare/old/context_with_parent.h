#ifndef POINCARE_CONTEXT_WITH_PARENT_H
#define POINCARE_CONTEXT_WITH_PARENT_H

#include <assert.h>
#include <poincare/expression.h>

#include "context.h"

namespace Poincare {

class ContextWithParent : public Context {
 public:
  ContextWithParent(Context* parentContext) : m_parentContext(parentContext) {}

  // Context
  const Internal::Tree* expressionForSymbolAbstract(
      const Internal::Tree* symbol) override {
    assert(m_parentContext);
    return m_parentContext->expressionForSymbolAbstract(symbol);
  }

  SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                 int length) override {
    assert(m_parentContext);
    return m_parentContext->expressionTypeForIdentifier(identifier, length);
  }
  bool setExpressionForSymbolAbstract(const Internal::Tree* expression,
                                      const Internal::Tree* symbol) override {
    assert(m_parentContext);
    return m_parentContext->setExpressionForSymbolAbstract(expression, symbol);
  }

 private:
  Context* m_parentContext;
};

}  // namespace Poincare

#endif
