#include "sequence_store.h"
#include <ion/storage/container.h>
extern "C" {
#include <assert.h>
#include <stddef.h>
}

using namespace Escher;

namespace Shared {

const char * SequenceStore::firstAvailableName(size_t * nameIndex) {
  // Choose available name
  size_t currentNameIndex = 0;
  while (currentNameIndex < k_maxNumberOfSequences) {
    const char * name = k_sequenceNames[currentNameIndex];
    if (Ion::Storage::Container::sharedStorage()->recordBaseNamedWithExtension(name, Ion::Storage::seqExtension).isNull()) {
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
  size_t nameIndex;
  const char * name = firstAvailableName(&nameIndex);
  assert(name);
  // Choose the corresponding color
  assert(nameIndex < Palette::numberOfDataColors());
  KDColor color = Palette::DataColor[nameIndex];
  Sequence::RecordDataBuffer data(color);
  // m_sequences
  return Ion::Storage::Container::sharedStorage()->createRecordWithExtension(name, modelExtension(), &data, sizeof(data));
}

int SequenceStore::sequenceIndexForName(char name) {
  for (int i = 0; i < k_maxNumberOfSequences; i++) {
    if (k_sequenceNames[i][0] == name) {
      return i;
    }
  }
  assert(false);
  return 0;
}

Shared::ExpressionModelHandle * SequenceStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  int index = cacheIndex;
  if (!record.isNull()) {
    index = SequenceStore::sequenceIndexForName(record.fullName()[0]);
  }
  m_sequences[index] = Sequence(record);
  return &m_sequences[index];
}

Shared::ExpressionModelHandle * SequenceStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_sequences[cacheIndex];
}

}
