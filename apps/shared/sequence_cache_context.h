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
                       int sequenceBeingComputed, bool independent);

 private:
  const Poincare::Expression protectedExpressionForSymbolAbstract(
      const Poincare::SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;
  SequenceContext* m_sequenceContext;
  int m_sequenceBeingComputed;
  bool m_independent;
};

}  // namespace Shared

#endif
