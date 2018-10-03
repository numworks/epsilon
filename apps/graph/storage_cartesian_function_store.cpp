#include "storage_cartesian_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

namespace Graph {

//constexpr const char * StorageCartesianFunctionStore::k_functionNames[k_maxNumberOfFunctions];

StorageCartesianFunction StorageCartesianFunctionStore::NullModel() {
  return StorageCartesianFunction("", KDColorBlack);
}

StorageCartesianFunctionStore::StorageCartesianFunctionStore() :
  Shared::StorageFunctionStore<StorageCartesianFunction>()
{
  //addEmptyModel();
}

char StorageCartesianFunctionStore::symbol() const {
  return 'x';
}

void StorageCartesianFunctionStore::removeAll() {
  StorageFunctionStore::removeAll();
  addEmptyModel();
}


}
