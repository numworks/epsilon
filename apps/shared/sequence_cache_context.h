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
  SequenceCacheContext(SequenceContext * sequenceContext, int forbiddenSequenceIndex);
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) override;
  void setValueForSymbol(T value, const Poincare::Symbol & symbol);
private:
  int nameIndexForSymbol(const Poincare::Symbol & symbol);
  int rankIndexForSymbol(const Poincare::Symbol & symbol);
  T m_values[SequenceStore::k_maxNumberOfSequences][SequenceStore::k_maxRecurrenceDepth];
  SequenceContext * m_sequenceContext;
  int m_forbiddenSequenceIndex;
};

}

#endif
