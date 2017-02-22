#include "cartesian_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

namespace Graph {

constexpr int CartesianFunctionStore::k_numberOfDefaultColors;
constexpr KDColor CartesianFunctionStore::k_defaultColors[k_numberOfDefaultColors];
constexpr const char * CartesianFunctionStore::k_functionNames[k_maxNumberOfFunctions];

CartesianFunctionStore::CartesianFunctionStore() :
  Shared::FunctionStore()
{
  addEmptyFunction();
}

uint32_t CartesianFunctionStore::storeChecksum() {
  size_t dataLengthInBytes = m_numberOfFunctions*sizeof(CartesianFunction);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)m_functions, dataLengthInBytes>>2);
}

CartesianFunction * CartesianFunctionStore::functionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  return &m_functions[i];
}

CartesianFunction * CartesianFunctionStore::activeFunctionAtIndex(int i) {
  return (CartesianFunction *)Shared::FunctionStore::activeFunctionAtIndex(i);
}

CartesianFunction * CartesianFunctionStore::definedFunctionAtIndex(int i) {
  return (CartesianFunction *)Shared::FunctionStore::definedFunctionAtIndex(i);
}

CartesianFunction * CartesianFunctionStore::addEmptyFunction() {
  assert(m_numberOfFunctions < k_maxNumberOfFunctions);
  const char * name = firstAvailableName();
  KDColor color = firstAvailableColor();
  CartesianFunction addedFunction = CartesianFunction(name, color);
  m_functions[m_numberOfFunctions] = addedFunction;
  CartesianFunction * result = &m_functions[m_numberOfFunctions];
  m_numberOfFunctions++;
  return result;
}

void CartesianFunctionStore::removeFunction(Shared::Function * f) {
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

int CartesianFunctionStore::maxNumberOfFunctions() {
  return k_maxNumberOfFunctions;
}

char CartesianFunctionStore::symbol() const {
  return 'x';
}

const char *  CartesianFunctionStore::firstAvailableName() {
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

const KDColor  CartesianFunctionStore::firstAvailableColor() {
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
