#ifndef APPS_SHARED_SEQUENCE_STORE_H
#define APPS_SHARED_SEQUENCE_STORE_H

#include "../shared/function_store.h"
#include "sequence.h"
#include <stdint.h>

namespace Shared {

class SequenceStore : public Shared::FunctionStore {
public:
  using Shared::FunctionStore::FunctionStore;
  /* Sequence Store hold all its Sequences in an array. The Sequence pointers
   * return by modelForRecord are therefore non-expirable. We choose to return
   * Sequence * instead of ExpiringPointer<Sequence>. */
  Sequence * modelForRecord(Ion::Storage::Record record) const { return static_cast<Sequence *>(privateModelForRecord(record)); }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  /* WARNING: after calling removeModel or removeAll, the sequence context
   * need to invalidate its cache as the sequences evaluations might have
   * changed */
  /* If the sequences have been defined in a unusual order, recordAtIndex(0)
   * may not be the record for u. */
  Ion::Storage::Record recordAtNameIndex(int i) const { return Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(k_sequenceNames[i], modelExtension()); }


  static int sequenceIndexForName(char name);
  static const char * firstAvailableName(size_t * nameIndex = nullptr);
  static constexpr const char * k_sequenceNames[] = {"u", "v", "w"};
  Sequence sequenceAtIndex(int i) { assert(i < MaxNumberOfSequences && i >= 0); return m_sequences[i]; }

private:
  int maxNumberOfMemoizedModels() const override { return MaxNumberOfSequences; }
  const char * modelExtension() const override { return Ion::Storage::seqExtension; }
  /* We don't use model memoization for two reasons:
   * - the number of Sequence is capped so we keep enough Sequences to store them all.
   * - evaluating sequences require to evaluate all of them at the same time
   *   (as they might be co-dependent) which doesn't suit our ring memoization.
   * To still make it work with the ExpressionModelStore, we force
   * setMemoizedModelAtIndex to store u, v and w sequences in this order in
   * m_sequences whatever the value of cacheIndex. We thus return a
   * ExpressionModelHandle pointer after setting the model as it won't be the
   * memoized model at cacheIndex. */
  Shared::ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Sequence m_sequences[MaxNumberOfSequences];
};

}

#endif
