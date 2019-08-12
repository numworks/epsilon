#include "expression_model_store.h"

namespace Shared {

ExpressionModelStore::ExpressionModelStore() :
  m_oldestMemoizedIndex(0)
{
}

int ExpressionModelStore::numberOfModels() const {
  return Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(modelExtension());
}

Ion::Storage::Record ExpressionModelStore::recordAtIndex(int i) const {
  return Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(modelExtension(), i);
}

ExpressionModelHandle * ExpressionModelStore::privateModelForRecord(Ion::Storage::Record record) const {
  for (int i = 0; i < maxNumberOfMemoizedModels(); i++) {
    if (!memoizedModelAtIndex(i)->isNull() && *memoizedModelAtIndex(i) == record) {
      return memoizedModelAtIndex(i);
    }
  }
  ExpressionModelHandle * result = setMemoizedModelAtIndex(m_oldestMemoizedIndex, record);
  m_oldestMemoizedIndex = (m_oldestMemoizedIndex+1) % maxNumberOfMemoizedModels();
  return result;
}


void ExpressionModelStore::removeAll() {
  Ion::Storage::sharedStorage()->destroyRecordsWithExtension(modelExtension());
}

void ExpressionModelStore::removeModel(Ion::Storage::Record record) {
  assert(!record.isNull());
  record.destroy();
}

void ExpressionModelStore::tidy() {
  resetMemoizedModelsExceptRecord();
}

int ExpressionModelStore::numberOfModelsSatisfyingTest(ModelTest test) const {
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

Ion::Storage::Record ExpressionModelStore::recordStatifyingTestAtIndex(int i, ModelTest test) const {
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

void ExpressionModelStore::resetMemoizedModelsExceptRecord(const Ion::Storage::Record record) const {
  Ion::Storage::Record emptyRecord;
  for (int i = 0; i < maxNumberOfMemoizedModels(); i++) {
    if (*memoizedModelAtIndex(i) != record) {
      setMemoizedModelAtIndex(i, emptyRecord);
    }
  }
}

}
