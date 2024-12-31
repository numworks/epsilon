#ifndef POINCARE_VARIABLE_CONTEXT_H
#define POINCARE_VARIABLE_CONTEXT_H

#include "context_with_parent.h"

namespace Poincare {

/* TODO: this context is used only by the parser, rework it ? */

class TreeVariableContext : public ContextWithParent {
 public:
  TreeVariableContext(const char* name = nullptr,
                      Context* parentContext = nullptr)
      : ContextWithParent(parentContext), m_name(name) {}

  // Context
  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) override;
  // The provided expression needs to outlive the VariableContext
  bool setExpressionForUserNamed(const Internal::Tree* expression,
                                 const Internal::Tree* symbol) override;

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) override;

 private:
  const char* m_name;
  const Internal::Tree* m_value;
};

}  // namespace Poincare

#endif
