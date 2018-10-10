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
  if (i >= m_firstMemoizedModelIndex+k_maxNumberOfMemoizedModels || i < m_firstMemoizedModelIndex) {
    // Change range of layout memoization
    int deltaIndex = i >= m_firstMemoizedModelIndex + k_maxNumberOfMemoizedModels ? i - k_maxNumberOfMemoizedModels + 1 - m_firstMemoizedModelIndex : i - m_firstMemoizedModelIndex;
    // Translate memoized models
    for (int i = 0; i < k_maxNumberOfMemoizedModels-1; i++) {
      int j = deltaIndex + i;
      if (j >= 0 && j < k_maxNumberOfMemoizedModels) {
        m_memoizedModelChecksum[i] = m_memoizedModelChecksum[j];
        moveMemoizedModel(i, j);
      } else {
        Ion::Storage::Record emptyRecord;
        setMemoizedModelAtIndex(i, emptyRecord);
      }
    }
    m_firstMemoizedModelIndex += deltaIndex;
  }
  assert(i-m_firstMemoizedModelIndex < k_maxNumberOfMemoizedModels);
  Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(modelExtension(), i);
  // Storage might have changed since last call
  if (memoizedModelAtIndex(i-m_firstMemoizedModelIndex)->isNull() || m_memoizedModelChecksum[i-m_firstMemoizedModelIndex] != record.checksum()) {
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
    assert(currentModelIndex < numberOfModels());
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

void StorageExpressionModelStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  m_memoizedModelChecksum[cacheIndex] = record.isNull() ? 0 : record.checksum();
  privateSetMemoizedModelAtIndex(cacheIndex, record);
}

}
