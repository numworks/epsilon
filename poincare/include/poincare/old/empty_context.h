#ifndef POINCARE_EMPTY_CONTEXT_H
#define POINCARE_EMPTY_CONTEXT_H

#include <assert.h>

#include "context.h"

namespace Poincare {

class EmptyContext : public Context {
 public:
  // Context
  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) const override {
    return UserNamedType::None;
  }

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override;
};

}  // namespace Poincare

#endif
