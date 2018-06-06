#include "function_store.h"
#include <assert.h>

namespace Shared {

FunctionStore::FunctionStore() :
  ExpressionModelStore()
{
}

Function * FunctionStore::activeFunctionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfModels);
  int index = 0;
  for (int k = 0; k < m_numberOfModels; k++) {
    Function * function = modelAtIndex(k);
    if (function->isActive() && function->isDefined()) {
      if (i == index) {
        return function;
      }
      index++;
    }
  }
  assert(false);
  return nullptr;
}

int FunctionStore::numberOfActiveFunctions() {
  int result = 0;
  for (int i = 0; i < m_numberOfModels; i++) {
    Function * function = modelAtIndex(i);
    if (function->isDefined() && function->isActive()) {
      result++;
    }
  }
  return result;
}

template<typename T>
T FunctionStore::firstAvailableAttribute(T attributes[], AttributeGetter<T> attribute) {
  for (int k = 0; k < maxNumberOfModels(); k++) {
    int j = 0;
    while  (j < m_numberOfModels) {
      if (attribute(modelAtIndex(j)) == attributes[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfModels) {
      return attributes[k];
    }
  }
  return attributes[0];
}

}

template char const* const Shared::FunctionStore::firstAvailableAttribute<char const* const>(char const* const*, char const* const (*)(Shared::Function*));
template KDColor const Shared::FunctionStore::firstAvailableAttribute<KDColor const>(KDColor const*, KDColor const (*)(Shared::Function*));
