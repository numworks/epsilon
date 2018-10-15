#include "storage_expression_model_store.h"

namespace Shared {

StorageExpressionModelStore::StorageExpressionModelStore() :
  m_firstMemoizedModelIndex(0)
{
}

int StorageExpressionModelStore::numberOfModels() const {
  return Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(modelExtension());
}

int StorageExpressionModelStore::numberOfDefinedModels() const {
  int result = 0;
  int i = 0;
  StorageExpressionModel * m = modelAtIndex(i++);
  while (!m->isNull()) {
    if (m->isDefined()) {
      result++;
    }
    m = modelAtIndex(i++);
  }
  return result;
}

StorageExpressionModel * StorageExpressionModelStore::modelAtIndex(int i) const {
  /* If i is out of the range of memoized indexes, we relocate the memoized
   * models to include the model i at the closest extremity of the array. */
  if (i >= m_firstMemoizedModelIndex + k_maxNumberOfMemoizedModels || i < m_firstMemoizedModelIndex) {
    int deltaIndex = i >= m_firstMemoizedModelIndex + k_maxNumberOfMemoizedModels ?
      i - (k_maxNumberOfMemoizedModels + m_firstMemoizedModelIndex) + 1:
      i - m_firstMemoizedModelIndex;
    // Relocate memoized models
    for (int i = 0; i < k_maxNumberOfMemoizedModels; i++) {
      int j = deltaIndex + i;
      if (j >= 0 && j < k_maxNumberOfMemoizedModels) {
        moveMemoizedModel(i, j);
      } else {
        // Fill new models with empty records
        Ion::Storage::Record emptyRecord;
        setMemoizedModelAtIndex(i, emptyRecord);
      }
    }
    m_firstMemoizedModelIndex += deltaIndex;
  }
  assert(i-m_firstMemoizedModelIndex < k_maxNumberOfMemoizedModels);
  uint32_t currentStorageChecksum = Ion::Storage::sharedStorage()->checksum();
  /* We have to build a new model if:
   * - the model has never been created
   * - Storage changed since last built. For instance, if f(x) = A+x, if A
   *   changes, we need to unmemoize f. */
  if (memoizedModelAtIndex(i-m_firstMemoizedModelIndex)->isNull() || currentStorageChecksum != m_storageChecksum) {
    if (currentStorageChecksum != m_storageChecksum) {
      resetMemoizedModels();
      m_storageChecksum = currentStorageChecksum;
    }
    Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(modelExtension(), i);
    setMemoizedModelAtIndex(i-m_firstMemoizedModelIndex, record);
  }
  return memoizedModelAtIndex(i-m_firstMemoizedModelIndex);
}

StorageExpressionModel * StorageExpressionModelStore::definedModelAtIndex(int i) const {
  assert(i >= 0 && i < numberOfDefinedModels());
  int index = 0;
  int currentModelIndex = 0;
  StorageExpressionModel * m = modelAtIndex(currentModelIndex++);
  while (!m->isNull()) {
    assert(currentModelIndex <= numberOfModels());
    if (m->isDefined()) {
      if (i == index) {
        return m;
      }
      index++;
    }
    m = modelAtIndex(currentModelIndex++);
  }
  assert(false);
  return nullptr;
}

void StorageExpressionModelStore::removeAll() {
  Ion::Storage::sharedStorage()->destroyRecordsWithExtension(modelExtension());
}

void StorageExpressionModelStore::removeModel(StorageExpressionModel * f) {
  assert(f != nullptr);
  f->destroy();
}

void StorageExpressionModelStore::tidy() {
  int i = 0;
  StorageExpressionModel * m = modelAtIndex(i++);
  while (!m->isNull()) {
    m->tidy();
    m = modelAtIndex(i++);
  }
}

void StorageExpressionModelStore::resetMemoizedModels() const {
  Ion::Storage::Record emptyRecord;
  for (int i = 0; i < k_maxNumberOfMemoizedModels; i++) {
    setMemoizedModelAtIndex(i, emptyRecord);
  }
}

}
