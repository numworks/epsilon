#ifndef SEQUENCE_SEQUENCE_STORE_H
#define SEQUENCE_SEQUENCE_STORE_H

#include "../shared/storage_function_store.h"
#include "sequence.h"
#include <stdint.h>
#include <escher.h>

namespace Sequence {

class SequenceStore : public Shared::StorageFunctionStore {
public:
  using Shared::StorageFunctionStore::StorageFunctionStore;
  char symbol() const override { return Sequence::Symbol(); }
  Shared::ExpiringPointer<Sequence> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<Sequence>(static_cast<Sequence *>(privateModelForRecord(record))); }
  /* WARNING: after calling removeModel or removeAll, the sequence context
   * need to invalidate its cache as the sequences evaluations might have
   * changed */
  int maxNumberOfModels() const override { return MaxNumberOfSequences; }

  static constexpr const char * k_sequenceNames[MaxNumberOfSequences] = {
    "u", "v"//, "w"
  };

private:
  const char * modelExtension() const override { return Sequence::extension; }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  /* We don't really use model memoization as the number of Sequence is limited
   * and we keep enough Sequences to store them all. */
  void setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::SingleExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Sequence m_sequences[MaxNumberOfSequences];
};

}

#endif
