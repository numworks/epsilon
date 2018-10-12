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
  assert(cacheIndex >= 0 && cacheIndex < k_maxNumberOfMemoizedModels);
  m_functions[cacheIndex] = StorageCartesianFunction(record);
}

void StorageCartesianFunctionStore::moveMemoizedModel(int newIndex, int previousIndex) const {
  assert(newIndex >= 0 && newIndex < k_maxNumberOfMemoizedModels);
  assert(previousIndex >= 0 && previousIndex < k_maxNumberOfMemoizedModels);
  m_functions[newIndex] = m_functions[previousIndex];
}

StorageExpressionModel * StorageCartesianFunctionStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < k_maxNumberOfMemoizedModels);
  return &m_functions[cacheIndex];
}

}
