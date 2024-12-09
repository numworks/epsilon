#ifndef APPS_SHARED_SEQUENCE_CONTEXT_H
#define APPS_SHARED_SEQUENCE_CONTEXT_H

#include <poincare/expression.h>
#include <poincare/old/context_with_parent.h>
#include <poincare/old/symbol.h>
#include <poincare/src/expression/sequence_cache.h>

#include "sequence_store.h"

namespace Shared {

class Sequence;

class SequenceContext : public Poincare::ContextWithParent {
  friend class Sequence;

 public:
  SequenceContext(Poincare::Context* parentContext,
                  SequenceStore* sequenceStore);

  /* u{n}, v{n} and w{n} must be parsed as sequences in the sequence app
   * so that u{n} can be defined as a function of v{n} without v{n} being
   * already defined.
   * So expressionTypeForIdentifier returns Type::Sequence for u, v and w,
   * and calls the parent context in other cases.
   * The other methods (setExpressionForUserNamed and
   * expressionForUserNamed) always call the parent context. */
  Poincare::Context::SymbolAbstractType expressionTypeForIdentifier(
      const char* identifier, int length) override;

  void tidyDownstreamPoolFrom(Poincare::PoolObject* treePoolCursor) override;
  SequenceStore* sequenceStore() { return m_sequenceStore; }
  bool sequenceIsNotComputable(int sequenceIndex);
  void resetCache() { cache()->resetCache(); }
  Sequence* sequenceAtNameIndex(int sequenceIndex) const;

 private:
  constexpr static int k_numberOfSequences =
      SequenceStore::k_maxNumberOfSequences;

  Poincare::Internal::SequenceCache* cache();
  SequenceStore* m_sequenceStore;
};

}  // namespace Shared

#endif
