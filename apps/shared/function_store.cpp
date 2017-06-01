#include "function_store.h"
#include <assert.h>

namespace Shared {

FunctionStore::FunctionStore() :
  m_numberOfFunctions(0)
{
}

Function * FunctionStore::activeFunctionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  int index = 0;
  for (int k = 0; k < m_numberOfFunctions; k++) {
    Function * function = functionAtIndex(k);
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

Function * FunctionStore::definedFunctionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  int index = 0;
  for (int k = 0; k < m_numberOfFunctions; k++) {
    if (functionAtIndex(k)->isDefined()) {
      if (i == index) {
        return functionAtIndex(k);
      }
      index++;
    }
  }
  assert(false);
  return nullptr;
}

int FunctionStore::numberOfFunctions() {
  return m_numberOfFunctions;
}

int FunctionStore::numberOfActiveFunctions() {
  int result = 0;
  for (int i = 0; i < m_numberOfFunctions; i++) {
    Function * function = functionAtIndex(i);
    if (function->isDefined() && function->isActive()) {
      result++;
    }
  }
  return result;
}

int FunctionStore::numberOfDefinedFunctions() {
  int result = 0;
  for (int i = 0; i < m_numberOfFunctions; i++) {
    if (functionAtIndex(i)->isDefined()) {
      result++;
    }
  }
  return result;
}

void FunctionStore::tidy() {
  for (int i = 0; i < m_numberOfFunctions; i++) {
    functionAtIndex(i)->tidy();
  }
}

}
