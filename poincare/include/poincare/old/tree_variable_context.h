#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include "context_with_parent.h"
#include "symbol_abstract.h"

namespace Poincare {

/* TODO: this context is used only by the parser, rework it ? */

class TreeVariableContext : public ContextWithParent {
 public:
  TreeVariableContext(const char* name = nullptr,
                      Context* parentContext = nullptr)
      : ContextWithParent(parentContext), m_name(name) {}
  /* Building a self referential context is a trick for parsing parametered
   * expression. */
  TreeVariableContext(SymbolAbstract symbol, Context* parentContext)
      : ContextWithParent(parentContext),
        m_name(symbol.name()),
        m_value(symbol) {}

  // Context
  SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                 int length) override;
  // The provided expression needs to outlive the VariableContext
  bool setExpressionForSymbolAbstract(const Internal::Tree* expression,
                                      const Internal::Tree* symbol) override;

  const Internal::Tree* expressionForSymbolAbstract(
      const Internal::Tree* symbol) override;

 private:
  const char* m_name;
  const Internal::Tree* m_value;
};

}  // namespace Poincare

#endif
