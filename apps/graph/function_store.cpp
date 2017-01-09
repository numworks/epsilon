#include "function_store.h"
extern "C" {
#include <assert.h>
}
#include <stddef.h>

namespace Graph {

constexpr int FunctionStore::k_numberOfDefaultColors;
constexpr KDColor FunctionStore::k_defaultColors[k_numberOfDefaultColors];
constexpr const char * FunctionStore::k_functionNames[k_maxNumberOfFunctions];

FunctionStore::FunctionStore() :
  m_numberOfFunctions(0)
{
  addEmptyFunction();
}

uint32_t FunctionStore::storeChecksum() {
  size_t dataLengthInBytes = m_numberOfFunctions*sizeof(Function);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  //return Ion::crc32((uint32_t *)m_functions, dataLengthInBytes>>2);
  return m_numberOfFunctions;
}

Function * FunctionStore::functionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  return &m_functions[i];
}

Function * FunctionStore::activeFunctionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  int index = 0;
  for (int k = 0; k < m_numberOfFunctions; k++) {
    if (m_functions[k].isActive() && m_functions[k].layout() != nullptr) {
      if (i == index) {
        return &m_functions[k];
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
    if (m_functions[k].layout() != nullptr) {
      if (i == index) {
        return &m_functions[k];
      }
      index++;
    }
  }
  assert(false);
  return nullptr;
}

Function * FunctionStore::addEmptyFunction() {
  assert(m_numberOfFunctions < k_maxNumberOfFunctions);
  const char * name = firstAvailableName();
  KDColor color = firstAvailableColor();
  Function addedFunction = Function(name, color);
  m_functions[m_numberOfFunctions] = addedFunction;
  Function * result = &m_functions[m_numberOfFunctions];
  m_numberOfFunctions++;
  return result;
}

void FunctionStore::removeFunction(Function * f) {
  int i = 0;
  while (&m_functions[i] != f && i < m_numberOfFunctions) {
    i++;
  }
  assert(i>=0 && i<m_numberOfFunctions);
  m_numberOfFunctions--;
  for (int j = i; j<m_numberOfFunctions; j++) {
    m_functions[j] = m_functions[j+1];
  }
}

int FunctionStore::numberOfFunctions() {
  return m_numberOfFunctions;
}

int FunctionStore::numberOfActiveFunctions() {
  int result = 0;
  for (int i = 0; i < m_numberOfFunctions; i++) {
    if (m_functions[i].layout() != nullptr && m_functions[i].isActive()) {
      result++;
    }
  }
  return result;
}

int FunctionStore::numberOfDefinedFunctions() {
  int result = 0;
  for (int i = 0; i < m_numberOfFunctions; i++) {
    if (m_functions[i].layout() != nullptr) {
      result++;
    }
  }
  return result;
}

const char *  FunctionStore::firstAvailableName() {
  for (int k = 0; k < k_maxNumberOfFunctions; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_functions[j].name() == k_functionNames[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return k_functionNames[k];
    }
  }
  return k_functionNames[0];
}

const KDColor  FunctionStore::firstAvailableColor() {
  for (int k = 0; k < k_numberOfDefaultColors; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_functions[j].color() == k_defaultColors[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return k_defaultColors[k];
    }
  }
  return k_defaultColors[0];
}

}
