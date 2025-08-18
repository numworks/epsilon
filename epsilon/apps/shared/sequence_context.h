#ifndef APPS_SHARED_SEQUENCE_CONTEXT_H
#define APPS_SHARED_SEQUENCE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/expression.h>

#include "sequence_cache.h"
#include "sequence_store.h"

namespace Shared {

class Sequence;

class SequenceContext final : public Poincare::ContextWithParent {
  friend class Sequence;

 public:
  SequenceContext(const Poincare::Context* parentContext,
                  SequenceStore* sequenceStore);

  // Context

  /* u{n}, v{n} and w{n} must be parsed as sequences in the sequence app
   * so that u{n} can be defined as a function of v{n} without v{n} being
   * already defined.
   * So expressionTypeForIdentifier returns Type::Sequence for u, v and w,
   * and calls the parent context in other cases.
   * The other methods (setExpressionForUserNamed and
   * expressionForUserNamed) always call the parent context. */
  Poincare::Context::UserNamedType expressionTypeForIdentifier(
      const char* identifier, int length) const override;

  // SequenceContext

  SequenceStore* sequenceStore() { return m_sequenceStore; }
  bool sequenceIsNotComputable(const Poincare::Context& context,
                               int sequenceIndex) const;
  void resetCache() const { cache()->resetCache(); }
  Sequence* sequenceAtNameIndex(int sequenceIndex) const;

 private:
  constexpr static int k_numberOfSequences =
      SequenceStore::k_maxNumberOfSequences;

  SequenceCache* cache() const;

  SequenceStore* m_sequenceStore;
};

}  // namespace Shared

#endif
