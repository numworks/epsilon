#include "storage_cartesian_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

using namespace Shared;

namespace Graph {

StorageCartesianFunctionStore::StorageCartesianFunctionStore() :
  Shared::StorageFunctionStore()
{
  Ion::Storage::Record::ErrorStatus error = addEmptyModel();
  assert(error == Ion::Storage::Record::ErrorStatus::None);
}

void StorageCartesianFunctionStore::removeAll() {
  StorageFunctionStore::removeAll();
  Ion::Storage::Record::ErrorStatus error = addEmptyModel();
  assert(error == Ion::Storage::Record::ErrorStatus::None);
}

Ion::Storage::Record::ErrorStatus StorageCartesianFunctionStore::addEmptyModel() {
  Ion::Storage::Record::ErrorStatus error;
  StorageCartesianFunction newModel = StorageCartesianFunction::NewModel(&error);
  return error;
}

void StorageCartesianFunctionStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < k_maxNumberOfMemoizedModels);
  m_functions[cacheIndex] = StorageCartesianFunction(record);
}

void StorageCartesianFunctionStore::moveMemoizedModel(int previousIndex, int nextIndex) const {
  assert(nextIndex >= 0 && nextIndex < k_maxNumberOfMemoizedModels);
  assert(previousIndex >= 0 && previousIndex < k_maxNumberOfMemoizedModels);
  m_functions[nextIndex] = m_functions[previousIndex];
}

StorageExpressionModel * StorageCartesianFunctionStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < k_maxNumberOfMemoizedModels);
  return &m_functions[cacheIndex];
}

}
