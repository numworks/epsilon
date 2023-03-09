#ifndef SHARED_SEQUENCE_CACHE_CONTEXT_H
#define SHARED_SEQUENCE_CACHE_CONTEXT_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>

#include "sequence_context.h"

namespace Shared {

template <typename T>
class SequenceCacheContext : public Poincare::ContextWithParent {
 public:
  SequenceCacheContext(SequenceContext* sequenceContext,
                       int forbiddenSequenceIndex);
  void setValueForSymbol(T value, const Poincare::Symbol& symbol);

 private:
  const Poincare::Expression protectedExpressionForSymbolAbstract(
      const Poincare::SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;
  int nameIndexForSymbol(const Poincare::Symbol& symbol);
  int rankIndexForSymbol(const Poincare::Symbol& symbol);

  // m_values = {{u(n), u(n+1)}, {v(n), v(n+1)}, {w(n), w(n+1)}}
  T m_values[SequenceStore::k_maxNumberOfSequences]
            [SequenceStore::k_maxRecurrenceDepth];
  SequenceContext* m_sequenceContext;
  int m_forbiddenSequenceIndex;
};

}  // namespace Shared

#endif
