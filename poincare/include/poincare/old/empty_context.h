#ifndef POINCARE_EMPTY_CONTEXT_H
#define POINCARE_EMPTY_CONTEXT_H

#include <assert.h>

#include "context.h"

namespace Poincare {

class Expression;
class ContextWithParent;

class EmptyContext : public Context {
 public:
  // Context
  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) override {
    return UserNamedType::None;
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
