#ifndef POINCARE_INTEGER_VARIABLE_CONTEXT_H
#define POINCARE_INTEGER_VARIABLE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/float.h>

namespace Poincare {

class IntegerVariableContext : public ContextWithParent {
public:
  IntegerVariableContext(const char * name, Context * parentContext) :
    ContextWithParent(parentContext),
    m_name(name)
  {}

  SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) override { return strcmp(m_name, identifier) == 0 ? SymbolAbstractType::Integer : ContextWithParent::expressionTypeForIdentifier(identifier, length); }

private:
  const char * m_name;
};

}

#endif
