#include "storage_cartesian_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

using namespace Shared;

namespace Graph {

Ion::Storage::Record::ErrorStatus StorageCartesianFunctionStore::addEmptyModel() {
  Ion::Storage::Record::ErrorStatus error;
  StorageCartesianFunction newModel = StorageCartesianFunction::NewModel(&error);
  return error;
}

void StorageCartesianFunctionStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_functions[cacheIndex] = StorageCartesianFunction(record);
}

SingleExpressionModelHandle * StorageCartesianFunctionStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_functions[cacheIndex];
}

}
