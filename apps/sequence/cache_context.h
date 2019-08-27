#ifndef SEQUENCE_CACHE_CONTEXT_H
#define SEQUENCE_CACHE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>
#include "sequence_context.h"

namespace Sequence {

template<typename T>
class CacheContext : public Poincare::Context {
public:
  CacheContext(Poincare::Context * parentContext);
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone) override;
  void setExpressionForSymbolAbstract(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol, Poincare::Context * context) override;
  void setValueForSymbol(T value, const Poincare::Symbol & symbol);
private:
  int nameIndexForSymbol(const Poincare::Symbol & symbol);
  int rankIndexForSymbol(const Poincare::Symbol & symbol);
  T m_values[MaxNumberOfSequences][MaxRecurrenceDepth];
  Context * m_parentContext;
};

}

#endif
