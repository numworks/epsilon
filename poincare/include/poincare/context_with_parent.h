#ifndef POINCARE_CONTEXT_WITH_PARENT_H
#define POINCARE_CONTEXT_WITH_PARENT_H

#include <assert.h>
#include <poincare/context.h>
#include <poincare/expression.h>

namespace Poincare {

class ContextWithParent : public Context {
 public:
  ContextWithParent(Context* parentContext) : m_parentContext(parentContext) {
    assert(parentContext);
  }

  // Context
  SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                 int length) override {
    return m_parentContext->expressionTypeForIdentifier(identifier, length);
  }
  bool setExpressionForSymbolAbstract(const Expression& expression,
                                      const SymbolAbstract& symbol) override {
    return m_parentContext->setExpressionForSymbolAbstract(expression, symbol);
  }

 protected:
  const Expression protectedExpressionForSymbolAbstract(
      const SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override {
    return m_parentContext->protectedExpressionForSymbolAbstract(
        symbol, clone,
        lastDescendantContext == nullptr ? this : lastDescendantContext);
  }

 private:
  Context* m_parentContext;
};

}  // namespace Poincare

#endif
