#include "storage_expression_model_store.h"

namespace Shared {

StorageExpressionModelStore::StorageExpressionModelStore() :
  m_oldestMemoizedIndex(0)
{
}

int StorageExpressionModelStore::numberOfModels() const {
  return Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(modelExtension());
}

Ion::Storage::Record StorageExpressionModelStore::recordAtIndex(int i) const {
  return Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(modelExtension(), i);
}

ExpressionModelHandle * StorageExpressionModelStore::privateModelForRecord(Ion::Storage::Record record) const {
  for (int i = 0; i < maxNumberOfMemoizedModels(); i++) {
    if (!memoizedModelAtIndex(i)->isNull() && *memoizedModelAtIndex(i) == record) {
      return memoizedModelAtIndex(i);
    }
  }
  setMemoizedModelAtIndex(m_oldestMemoizedIndex, record);
  ExpressionModelHandle * result = memoizedModelAtIndex(m_oldestMemoizedIndex);
  m_oldestMemoizedIndex = (m_oldestMemoizedIndex+1) % maxNumberOfMemoizedModels();
  return result;
}


void StorageExpressionModelStore::removeAll() {
  Ion::Storage::sharedStorage()->destroyRecordsWithExtension(modelExtension());
}

void StorageExpressionModelStore::removeModel(Ion::Storage::Record record) {
  assert(!record.isNull());
  record.destroy();
}

void StorageExpressionModelStore::tidy() {
  resetMemoizedModelsExceptRecord();
}

int StorageExpressionModelStore::numberOfModelsSatisfyingTest(ModelTest test) const {
  int result = 0;
  int i = 0;
  ExpressionModelHandle * m = privateModelForRecord(recordAtIndex(i++));
  while (!m->isNull()) {
    if (test(m)) {
      result++;
    }
    m = privateModelForRecord(recordAtIndex(i++));
  }
  return result;
}

Ion::Storage::Record StorageExpressionModelStore::recordStatifyingTestAtIndex(int i, ModelTest test) const {
  assert(i >= 0 && i < numberOfDefinedModels());
  int index = 0;
  int currentModelIndex = 0;
  Ion::Storage::Record r = recordAtIndex(currentModelIndex++);
  ExpressionModelHandle * m = privateModelForRecord(r);
  while (!m->isNull()) {
    assert(currentModelIndex <= numberOfModels());
    if (test(m)) {
      if (i == index) {
        return r;
      }
      index++;
    }
    r = recordAtIndex(currentModelIndex++);
    m = privateModelForRecord(r);
  }
  assert(false);
  return Ion::Storage::Record();
}

void StorageExpressionModelStore::resetMemoizedModelsExceptRecord(const Ion::Storage::Record record) const {
  Ion::Storage::Record emptyRecord;
  for (int i = 0; i < maxNumberOfMemoizedModels(); i++) {
    if (*memoizedModelAtIndex(i) != record) {
      setMemoizedModelAtIndex(i, emptyRecord);
    }
  }
}

}
