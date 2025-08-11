#ifndef APPS_SHARED_SEQUENCE_CONTEXT_H
#define APPS_SHARED_SEQUENCE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/expression.h>
#include <poincare/variable_store.h>

#include "sequence_cache.h"
#include "sequence_store.h"

namespace Shared {

class Sequence;

class SequenceContext final : public Poincare::VariableStore {
  friend class Sequence;

 public:
  SequenceContext(Poincare::VariableStore* parentStore,
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

  const Poincare::Internal::Tree* expressionForUserNamed(
      const Poincare::Internal::Tree* symbol) const override {
    assert(m_parentStore);
    return m_parentStore->expressionForUserNamed(symbol);
  }

  double approximateSequenceAtRank(const char* identifier,
                                   int rank) const override {
    assert(m_parentStore);
    return m_parentStore->approximateSequenceAtRank(identifier, rank);
  }

  // VariableStore

  bool setExpressionForUserNamed(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* symbol) override {
    assert(m_parentStore);
    return m_parentStore->setExpressionForUserNamed(expression, symbol);
  }

  // SequenceContext

  SequenceStore* sequenceStore() { return m_sequenceStore; }
  bool sequenceIsNotComputable(Poincare::Context* ctx, int sequenceIndex);
  void resetCache() { cache()->resetCache(); }
  Sequence* sequenceAtNameIndex(int sequenceIndex) const;

 private:
  constexpr static int k_numberOfSequences =
      SequenceStore::k_maxNumberOfSequences;

  SequenceCache* cache();

  VariableStore* m_parentStore;
  SequenceStore* m_sequenceStore;
};

}  // namespace Shared

#endif
