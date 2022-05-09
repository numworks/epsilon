#include "expression_model_store.h"

namespace Shared {

ExpressionModelStore::ExpressionModelStore() :
  m_oldestMemoizedIndex(0)
{
}

int ExpressionModelStore::numberOfModels() const {
  return Ion::Storage::Container::sharedStorage()->numberOfRecordsWithExtension(modelExtension());
}

Ion::Storage::Record ExpressionModelStore::recordAtIndex(int i) const {
  return Ion::Storage::Container::sharedStorage()->recordWithExtensionAtIndex(modelExtension(), i);
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
  Ion::Storage::Container::sharedStorage()->destroyRecordsWithExtension(modelExtension());
}

void ExpressionModelStore::removeModel(Ion::Storage::Record record) {
  assert(!record.isNull());
  record.destroy();
}

void ExpressionModelStore::tidyDownstreamPoolFrom(char * treePoolCursor) {
  for (int i = 0; i < maxNumberOfMemoizedModels(); i++) {
    memoizedModelAtIndex(i)->tidyDownstreamPoolFrom(treePoolCursor);
  }
}

int ExpressionModelStore::numberOfModelsSatisfyingTest(ModelTest test, void * context) const {
  int count = 0;
  int index = 0;
  Ion::Storage::Record record;
  do {
    record = recordAtIndex(index++);
    if (record.isNull()) {
      break;
    }
    if (test(privateModelForRecord(record), context)) {
      count++;
    }
  } while (true);
  return count;
}

Ion::Storage::Record ExpressionModelStore::recordSatisfyingTestAtIndex(int i, ModelTest test, void * context) const {
  assert(0 <= i && i < numberOfModelsSatisfyingTest(test, context));
  int count = 0;
  int index = 0;
  Ion::Storage::Record record;
  do {
    record = recordAtIndex(index++);
    if (test(privateModelForRecord(record), context)) {
      if (i == count) {
        break;
      }
      count++;
    }
  } while (true);
  return record;
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
