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

  /* This function always returns Type::Sequence for u, v and w, so that they
   * are always parsed as sequences, even if they are not defined yet.
   * Their Tree* is always nullptr though, even if they are defined, because
   * sequences don't have a Tree* and can only be approximated.
   * TODO: We could add support for explicit sequences by returning their
   * expression here */
  SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override;

 private:
  constexpr static int k_numberOfSequences =
      SequenceStore::k_maxNumberOfSequences;

  SequenceCache* cache() const;

  const SequenceStore* m_sequenceStore;
};

}  // namespace Shared
