#pragma once

#include "context_with_parent.h"
#include "helpers/symbol.h"
#include "user_expression.h"

namespace Poincare {

class PoolVariableContext : public ContextWithParent {
 public:
  PoolVariableContext(const char* name, UserExpression expression,
                      const Context* parentContext)
      : ContextWithParent(parentContext), m_name(name), m_value(expression) {}
  /* Building a self referential context is a trick for parsing parametered
   * expression. */
  PoolVariableContext(UserExpression symbol, const Context* parentContext)
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
  UserExpression m_value;
};

}  // namespace Poincare
