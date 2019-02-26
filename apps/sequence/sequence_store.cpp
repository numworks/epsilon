#include "sequence_store.h"
#include <ion/storage.h>
extern "C" {
#include <assert.h>
#include <stddef.h>
}

namespace Sequence {

constexpr const char * SequenceStore::k_sequenceNames[MaxNumberOfSequences];

Ion::Storage::Record::ErrorStatus SequenceStore::addEmptyModel() {
  // Choose available name
  const char * name;
  int currentNameIndex = 0;
  while (currentNameIndex < MaxNumberOfSequences) {
    name = k_sequenceNames[currentNameIndex];
    if (Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(name, Sequence::extension).isNull()) {
      break;
    }
    currentNameIndex++;
  }
  assert(currentNameIndex < MaxNumberOfSequences);
  // Choose the corresponding color
  KDColor color = Palette::DataColor[currentNameIndex];
  Sequence::SequenceRecordData data(color);
  // m_sequences
  return Ion::Storage::sharedStorage()->createRecordWithExtension(name, Sequence::extension, &data, sizeof(data));
}

void SequenceStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_sequences[cacheIndex] = Sequence(record);
}

Shared::SingleExpressionModelHandle * SequenceStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_sequences[cacheIndex];
}

}
