#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include "context_with_parent.h"

namespace Poincare {

/* TODO: this context is used only by the parser, rework it ? */

class TreeVariableContext : public ContextWithParent {
 public:
  TreeVariableContext(const char* name, const Internal::Tree* value,
                      const Context* parentContext = nullptr)
      : ContextWithParent(parentContext), m_name(name), m_value(value) {}
  TreeVariableContext(const char* name = nullptr,
                      const Context* parentContext = nullptr)
      : TreeVariableContext(name, nullptr, parentContext) {}

  // Context
  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) const override;

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override;

 private:
  const char* m_name;
  const Internal::Tree* m_value;
};

}  // namespace Poincare

#endif
