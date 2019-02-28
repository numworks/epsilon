#include "sequence_store.h"
#include <ion/storage.h>
extern "C" {
#include <assert.h>
#include <stddef.h>
}

namespace Sequence {

constexpr const char * SequenceStore::k_sequenceNames[MaxNumberOfSequences];

const char * SequenceStore::firstAvailableName(int * nameIndex) {
  // Choose available name
  int currentNameIndex = 0;
  while (currentNameIndex < MaxNumberOfSequences) {
    const char * name = k_sequenceNames[currentNameIndex];
    if (Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(name, Shared::GlobalContext::seqExtension).isNull()) {
      if (nameIndex) {
        *nameIndex = currentNameIndex;
      }
      return name;
    }
    currentNameIndex++;
  }
  return nullptr;
}

Ion::Storage::Record::ErrorStatus SequenceStore::addEmptyModel() {
  // Choose available name
  int nameIndex;
  const char * name = firstAvailableName(&nameIndex);
  assert(name);
  // Choose the corresponding color
  KDColor color = Palette::DataColor[nameIndex];
  Sequence::SequenceRecordData data(color);
  // m_sequences
  return Ion::Storage::sharedStorage()->createRecordWithExtension(name, modelExtension(), &data, sizeof(data));
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
