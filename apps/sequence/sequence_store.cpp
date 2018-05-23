#include "sequence_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

namespace Sequence {

constexpr KDColor SequenceStore::k_defaultColors[MaxNumberOfSequences];
constexpr const char * SequenceStore::k_sequenceNames[MaxNumberOfSequences];

uint32_t SequenceStore::storeChecksum() {
  size_t dataLengthInBytes = MaxNumberOfSequences*sizeof(uint32_t);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  uint32_t checksums[MaxNumberOfSequences];
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    checksums[i] = m_sequences[i].checksum();
  }
  return Ion::crc32((uint32_t *)checksums, dataLengthInBytes/sizeof(uint32_t));
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
