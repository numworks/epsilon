#pragma once

#include <poincare/context_with_parent.h>
#include <poincare/sequence_approximation_helper.h>
#include <poincare/symbol_context.h>

#include "sequence_cache.h"
#include "sequence_store.h"

namespace Shared {

class Sequence;

class SequenceContext final
    : public Poincare::ContextWithParent,
      public Poincare::SequenceApproximationHelper::ContextInterface {
  friend class Sequence;

 public:
  SequenceContext(const Poincare::SymbolContext* parentContext,
                  const SequenceStore* sequenceStore);

  // SequenceApproximationHelper::Context
  double approximateSequenceAtRank(const char* identifier,
                                   int rank) const override;

  // SequenceContext

  bool sequenceIsNotComputable(int sequenceIndex) const;
  void resetCache() const { cache()->resetCache(); }
  const Sequence* sequenceAtNameIndex(int sequenceIndex) const;

 protected:
  // Symbol Context override

  /* This function always returns {type: Type::Sequence, expression: nullptr}
   * for u, v and w.
   * - Their type is always Sequence is so that u{n} can be defined as a
   * function of v{n} without v{n} being already defined.
   * - Their expression is always nullptr because sequence can only be
   * approximated.
   * TODO: We could add support for explicit sequences by returning their
   * expression here. We would also need to change the assert in
   * SymbolContext::expressionForSymbol accordingly. */
  SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override;

 private:
  constexpr static int k_numberOfSequences =
      SequenceStore::k_maxNumberOfSequences;

  SequenceCache* cache() const;

  const SequenceStore* m_sequenceStore;
};

}  // namespace Shared
