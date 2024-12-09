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
  bool setExpressionForUserNamed(const Internal::Tree* expression,
                                 const Internal::Tree* symbol) override {
    assert(false);
    return false;
  }

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) override;
};

}  // namespace Poincare

#endif
