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
  bool setExpressionForSymbolAbstract(const JuniorExpression& expression,
                                      const SymbolAbstract& symbol) override {
    assert(false);
    return false;
  }

 protected:
  const JuniorExpression protectedExpressionForSymbolAbstract(
      const SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;
};

}  // namespace Poincare

#endif
