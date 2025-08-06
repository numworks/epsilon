#ifndef POINCARE_POOL_VARIABLE_CONTEXT_H
#define POINCARE_POOL_VARIABLE_CONTEXT_H

#include <poincare/expression.h>
#include <poincare/helpers/symbol.h>

#include "context_with_parent.h"

namespace Poincare {

class PoolVariableContext : public ContextWithParent {
 public:
  PoolVariableContext(const char* name, UserExpression expression,
                      Context* parentContext)
      : ContextWithParent(parentContext), m_name(name), m_value(expression) {}
  /* Building a self referential context is a trick for parsing parametered
   * expression. */
  PoolVariableContext(UserExpression symbol, Context* parentContext)
      : ContextWithParent(parentContext),
        m_name(SymbolHelper::GetName(symbol)),
        m_value(symbol) {}

  // Context
  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) const override;

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override;

 private:
  const char* m_name;
  Expression m_value;
};

}  // namespace Poincare

#endif
