#ifndef POINCARE_EMPTY_CONTEXT_H
#define POINCARE_EMPTY_CONTEXT_H

#include <assert.h>

#include "context.h"

namespace Poincare {

class JuniorExpression;
class ContextWithParent;

class EmptyContext : public Context {
 public:
  // Context
  SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                 int length) override {
    return SymbolAbstractType::None;
  }
  bool setExpressionForSymbolAbstract(const Internal::Tree* expression,
                                      const Internal::Tree* symbol) override {
    assert(false);
    return false;
  }

 protected:
  const Internal::Tree* protectedExpressionForSymbolAbstract(
      const Internal::Tree* symbol,
      ContextWithParent* lastDescendantContext) override;
};

}  // namespace Poincare

#endif
