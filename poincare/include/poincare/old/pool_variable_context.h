#ifndef POINCARE_POOL_VARIABLE_CONTEXT_H
#define POINCARE_POOL_VARIABLE_CONTEXT_H

#include "context_with_parent.h"
#include "symbol_abstract.h"

namespace Poincare {

class PoolVariableContext : public ContextWithParent {
 public:
  PoolVariableContext(const char* name = nullptr,
                      Context* parentContext = nullptr)
      : ContextWithParent(parentContext), m_name(name) {}
  /* Building a self referential context is a trick for parsing parametered
   * expression. */
  PoolVariableContext(SymbolAbstract symbol, Context* parentContext)
      : ContextWithParent(parentContext),
        m_name(symbol.name()),
        m_value(symbol) {}

  template <typename T>
  void setApproximationForVariable(T value);
  // Context
  SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                 int length) override;
  bool setExpressionForUserNamed(const Internal::Tree* expression,
                                 const Internal::Tree* symbol) override;

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) override;

 private:
  const char* m_name;
  Expression m_value;
};

}  // namespace Poincare

#endif
