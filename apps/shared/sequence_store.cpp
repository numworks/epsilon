#include "sequence_store.h"

#include <ion/storage/file_system.h>

#include <algorithm>
extern "C" {
#include <assert.h>
#include <stddef.h>
}

using namespace Escher;

namespace Shared {

const char* SequenceStore::FirstAvailableName(size_t* nameIndex) {
  // Choose available name
  size_t currentNameIndex = 0;
  while (currentNameIndex < k_maxNumberOfSequences) {
    const char* name = k_sequenceNames[currentNameIndex];
    if (Ion::Storage::FileSystem::sharedFileSystem
            ->recordBaseNamedWithExtension(name, Ion::Storage::seqExtension)
            .isNull()) {
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
  const char* name = FirstAvailableName(&nameIndex);
  assert(name);
  // Choose the corresponding color
  assert(nameIndex < Palette::numberOfDataColors());
  KDColor color = Palette::DataColor[nameIndex];
  Sequence::RecordDataBuffer data(color);
  // m_sequences
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      name, modelExtension(), &data, sizeof(data));
}

int SequenceStore::SequenceIndexForName(char name) {
  for (int i = 0; i < k_maxNumberOfSequences; i++) {
    if (k_sequenceNames[i][0] == name) {
      return i;
    }
  }
  assert(false);
  return 0;
}

Shared::ExpressionModelHandle* SequenceStore::setMemoizedModelAtIndex(
    int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  int index = cacheIndex;
  if (!record.isNull()) {
    index = SequenceStore::SequenceIndexForName(record.fullName()[0]);
  }
  m_sequences[index] = Sequence(record);
  return &m_sequences[index];
}

Shared::ExpressionModelHandle* SequenceStore::memoizedModelAtIndex(
    int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_sequences[cacheIndex];
}

float SequenceStore::smallestInitialRank() const {
  int smallestRank = Shared::Sequence::k_maxInitialRank;
  for (int i = 0, end = numberOfActiveFunctions(); i < end; i++) {
    Ion::Storage::Record record = activeRecordAtIndex(i);
    Shared::Sequence* s = modelForRecord(record);
    smallestRank = std::min(s->initialRank(), smallestRank);
  }
  return smallestRank;
}

}  // namespace Shared
