#ifndef SHARED_SEQUENCE_CACHE_CONTEXT_H
#define SHARED_SEQUENCE_CACHE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>
#include "sequence_context.h"

namespace Shared {

template<typename T>
class SequenceCacheContext : public Poincare::ContextWithParent {
public:
  SequenceCacheContext(SequenceContext * sequenceContext);
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) override;
  void setValueForSymbol(T value, const Poincare::Symbol & symbol);
  void setNValue(int n) { m_nValue = n; }
private:
  int nameIndexForSymbol(const Poincare::Symbol & symbol);
  int rankIndexForSymbol(const Poincare::Symbol & symbol);
  T m_values[MaxNumberOfSequences][MaxRecurrenceDepth];
  int m_nValue;
  SequenceContext * m_sequenceContext;
};

}

#endif
