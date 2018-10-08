#include "storage_function_store.h"
#include "storage_cartesian_function.h"
#include <assert.h>

namespace Shared {

template<class T>
int StorageFunctionStore<T>::numberOfActiveFunctions() {
  int result = 0;
  int modelsCount = StorageExpressionModelStore<T>::numberOfModels();
  for (int i = 0; i < modelsCount; i++) {
    T function = StorageExpressionModelStore<T>::modelAtIndex(i);
    if (function.isDefined() && function.isActive()) {
      result++;
    }
  }
  return result;
}

template<class T>
T StorageFunctionStore<T>::activeFunctionAtIndex(int i) {
  assert(i >= 0 && i < numberOfActiveFunctions());
  int index = 0;
  int modelsCount = StorageExpressionModelStore<T>::numberOfModels();
  for (int k = 0; k < modelsCount; k++) {
    T function = StorageExpressionModelStore<T>::modelAtIndex(k);
    if (function.isActive() && function.isDefined()) {
      if (i == index) {
        return function;
      }
      index++;
    }
  }
  assert(false);
  return nullptr;
}

template<class T>
template<typename U>
U StorageFunctionStore<T>::firstAvailableAttribute(U attributes[], AttributeGetter<U> attribute) {
  int modelsCount = StorageExpressionModelStore<T>::numberOfModels();
  for (int k = 0; k < 4/*TODO*/; k++) {
    int j = 0;
    while  (j < modelsCount) {
      if (attribute(StorageExpressionModelStore<T>::modelAtIndex(j)) == attributes[k]) {
        break;
      }
      j++;
    }
    if (j == modelsCount) {
      return attributes[k];
    }
  }
  return attributes[0];
}

}

//TODO specify templates
template class Shared::StorageFunctionStore<Shared::StorageCartesianFunction>;
/*template char const* const Shared::StorageFunctionStore::firstAvailableAttribute<char const* const>(char const* const*, char const* const (*)(Shared::StorageFunction));
template KDColor const Shared::StorageFunctionStore::firstAvailableAttribute<KDColor const>(KDColor const*, KDColor const (*)(Shared::StorageFunction));*/
