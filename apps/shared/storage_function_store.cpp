#include "storage_function_store.h"
#include "storage_cartesian_function.h"
#include <assert.h>

namespace Shared {

uint32_t StorageFunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

int StorageFunctionStore::numberOfActiveFunctions() const {
  int result = 0;
  int i = 0;
  StorageFunction * function = modelAtIndex(i++);
  while (!function->isNull()) {
    if (function->isDefined() && function->isActive()) {
      result++;
    }
    function = modelAtIndex(i++);
  }
  return result;
}


StorageFunction * StorageFunctionStore::activeFunctionAtIndex(int i) const {
  assert(i >= 0 && i < numberOfDefinedModels());
  int index = 0;
  int currentModelIndex = 0;
  StorageFunction * function = modelAtIndex(currentModelIndex++);
  while (!function->isNull()) {
    assert(currentModelIndex <= numberOfModels());
    if (function->isActive() && function->isDefined()) {
      if (i == index) {
        return function;
      }
      index++;
    }
    function = modelAtIndex(currentModelIndex++);
  }
  assert(false);
  return nullptr;
}

}
