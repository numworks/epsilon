#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/float.h>
#include <poincare/symbol_abstract.h>

namespace Poincare {

class VariableContext : public ContextWithParent {
 public:
  VariableContext(const char* name, Context* parentContext)
      : ContextWithParent(parentContext), m_name(name) {}
  /* Building a self referential context is a trick for parsing parametered
   * expression. */
  VariableContext(SymbolAbstract symbol, Context* parentContext)
      : ContextWithParent(parentContext),
        m_name(symbol.name()),
        m_value(symbol) {}

  template <typename T>
  void setApproximationForVariable(T value);
  // Context
  SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                 int length) override;
  bool setExpressionForSymbolAbstract(const Expression& expression,
                                      const SymbolAbstract& symbol) override;

 protected:
  const Expression protectedExpressionForSymbolAbstract(
      const SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;

 private:
  const char* m_name;
  Expression m_value;
};

}  // namespace Poincare

#endif
