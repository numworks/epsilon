#ifndef SEQUENCE_CACHE_CONTEXT_H
#define SEQUENCE_CACHE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>
#include "sequence_context.h"

namespace Shared {

template<typename T>
class CacheContext : public Poincare::ContextWithParent {
public:
  CacheContext(Poincare::Context * parentContext);
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone) override;
  void setValueForSymbol(T value, const Poincare::Symbol & symbol);
  void setNValue(int n) { m_nValue = n; }
  void setSequenceContext(SequenceContext * sequenceContext) { m_sequenceContext = sequenceContext;}
private:
  int nameIndexForSymbol(const Poincare::Symbol & symbol);
  int rankIndexForSymbol(const Poincare::Symbol & symbol);
  T m_values[MaxNumberOfSequences][MaxRecurrenceDepth];
  int m_nValue;
  SequenceContext * m_sequenceContext;
};

}

#endif
