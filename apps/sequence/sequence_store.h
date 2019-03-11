#ifndef SEQUENCE_SEQUENCE_STORE_H
#define SEQUENCE_SEQUENCE_STORE_H

#include "../shared/storage_function_store.h"
#include "../shared/global_context.h"
#include "sequence.h"
#include <stdint.h>
#include <escher.h>

namespace Sequence {

class SequenceStore : public Shared::StorageFunctionStore {
public:
  using Shared::StorageFunctionStore::StorageFunctionStore;
  char symbol() const override { return Sequence::Symbol(); }
  char unknownSymbol() const override { return Poincare::Symbol::SpecialSymbols::UnknownN; }
  /* Sequence Store hold all its Sequences in an array. The Sequence pointers
   * return by modelForRecord are therefore non-expirable. We choose to return
   * Sequence * instead of ExpiringPointer<Sequence>. */
  Sequence * modelForRecord(Ion::Storage::Record record) const { return static_cast<Sequence *>(privateModelForRecord(record)); }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  /* WARNING: after calling removeModel or removeAll, the sequence context
   * need to invalidate its cache as the sequences evaluations might have
   * changed */
  int maxNumberOfModels() const override { return MaxNumberOfSequences; }

  static const char * firstAvailableName(int * nameIndex = nullptr);
  static constexpr const char * k_sequenceNames[MaxNumberOfSequences] = {
    "u", "v"//, "w"
  };

private:
  const char * modelExtension() const override { return Shared::GlobalContext::seqExtension; }
  /* We don't really use model memoization as the number of Sequence is limited
   * and we keep enough Sequences to store them all. */
  void setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::SingleExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Sequence m_sequences[MaxNumberOfSequences];
};

}

#endif
