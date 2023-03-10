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
                       int sequenceBeingComputed);
  void setValue(T value, int nameIndex, int depth);

 private:
  const Poincare::Expression protectedExpressionForSymbolAbstract(
      const Poincare::SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;
  int nameIndexForSymbol(const Poincare::Symbol& symbol);

  // m_values = {{u(n), u(n+1), u(n+2)}, {v(n), v(n+1), v(n+2)}, {w(n), w(n+1),
  // w(n+2)}}
  T m_values[SequenceStore::k_maxNumberOfSequences]
            [SequenceStore::k_maxRecurrenceDepth + 1];
  SequenceContext* m_sequenceContext;
  int m_sequenceBeingComputed;
};

}  // namespace Shared

#endif
