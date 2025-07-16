#ifndef POINCARE_CONTEXT_WITH_PARENT_H
#define POINCARE_CONTEXT_WITH_PARENT_H

#include <assert.h>

#include "context.h"

namespace Poincare {

class ContextWithParent : public Context {
 public:
  ContextWithParent(Context* parentContext) : m_parentContext(parentContext) {}

  // Context
  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override {
    assert(m_parentContext);
    return m_parentContext->expressionForUserNamed(symbol);
  }

  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) const override {
    assert(m_parentContext);
    return m_parentContext->expressionTypeForIdentifier(identifier, length);
  }
  bool setExpressionForUserNamed(const Internal::Tree* expression,
                                 const Internal::Tree* symbol) override {
    assert(m_parentContext);
    return m_parentContext->setExpressionForUserNamed(expression, symbol);
  }

  double approximateSequenceAtRank(const char* identifier,
                                   int rank) const override {
    assert(m_parentContext);
    return m_parentContext->approximateSequenceAtRank(identifier, rank);
  }

 private:
  Context* m_parentContext;
};

}  // namespace Poincare

#endif
