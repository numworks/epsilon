#include "sequence_store.h"
#include <ion/storage.h>
extern "C" {
#include <assert.h>
#include <stddef.h>
}

namespace Sequence {

constexpr const char * SequenceStore::k_sequenceNames[MaxNumberOfSequences];

uint32_t SequenceStore::storeChecksum() {
  assert((MaxNumberOfSequences*sizeof(uint32_t) & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  uint32_t checksums[MaxNumberOfSequences];
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    checksums[i] = m_sequences[i].checksum();
  }
  constexpr int checksumsStoreStorageSize = 2;
  uint32_t checksumsStoreStorage[checksumsStoreStorageSize];
  checksumsStoreStorage[0] = Ion::crc32((uint32_t *)checksums, MaxNumberOfSequences);
  checksumsStoreStorage[1] = Ion::Storage::sharedStorage()->checksum();
  return Ion::crc32((uint32_t *)checksumsStoreStorage, checksumsStoreStorageSize);
}

char SequenceStore::symbol() const {
  return 'n';
}

Sequence * SequenceStore::emptyModel() {
  static Sequence addedSequence("", KDColorBlack);
  addedSequence = Sequence(firstAvailableName(), firstAvailableColor());
  return &addedSequence;
}

Sequence * SequenceStore::nullModel() {
  static Sequence emptyFunction("", KDColorBlack);
  return &emptyFunction;
}

void SequenceStore::setModelAtIndex(Shared::ExpressionModel * f, int i) {
  assert(i>=0 && i<m_numberOfModels);
  m_sequences[i] = *(static_cast<Sequence *>(f));
}

}
