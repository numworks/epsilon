#ifndef POINCARE_CONTEXT_WITH_PARENT_H
#define POINCARE_CONTEXT_WITH_PARENT_H

#include <assert.h>

#include "context.h"

namespace Poincare {

// If parentContext is nullptr, this context will behave like an empty context
class ContextWithParent : public Context {
 public:
  ContextWithParent(Context* parentContext) : m_parentContext(parentContext) {}

  // Context
  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override {
    return m_parentContext ? m_parentContext->expressionForUserNamed(symbol)
                           : nullptr;
  }

  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) const override {
    return m_parentContext ? m_parentContext->expressionTypeForIdentifier(
                                 identifier, length)
                           : UserNamedType::None;
  }

  double approximateSequenceAtRank(const char* identifier,
                                   int rank) const override {
    return m_parentContext
               ? m_parentContext->approximateSequenceAtRank(identifier, rank)
               : NAN;
  }

 private:
  Context* m_parentContext;
};

}  // namespace Poincare

#endif
