#include "cartesian_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

namespace Graph {

constexpr int CartesianFunctionStore::k_maxNumberOfFunctions;
constexpr KDColor CartesianFunctionStore::k_defaultColors[k_maxNumberOfFunctions];
constexpr const char * CartesianFunctionStore::k_functionNames[k_maxNumberOfFunctions];

CartesianFunctionStore::CartesianFunctionStore() :
  Shared::FunctionStore()
{
  addEmptyModel();
}

uint32_t CartesianFunctionStore::storeChecksum() {
  size_t dataLengthInBytes = k_maxNumberOfFunctions*sizeof(uint32_t);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  uint32_t checksums[k_maxNumberOfFunctions];
  for (int i = 0; i < k_maxNumberOfFunctions; i++) {
    checksums[i] = m_functions[i].checksum();
  }
  return Ion::crc32((uint32_t *)checksums, dataLengthInBytes/sizeof(uint32_t));
}


char CartesianFunctionStore::symbol() const {
  return 'x';
}

void CartesianFunctionStore::removeAll() {
  FunctionStore::removeAll();
  addEmptyModel();
}

CartesianFunction * CartesianFunctionStore::emptyModel() {
  static CartesianFunction addedFunction("", KDColorBlack);
  addedFunction = CartesianFunction(firstAvailableName(), firstAvailableColor());
  return &addedFunction;
}

CartesianFunction * CartesianFunctionStore::nullModel() {
  static CartesianFunction emptyFunction("", KDColorBlack);
  return &emptyFunction;
}

void CartesianFunctionStore::setModelAtIndex(Shared::ExpressionModel * e, int i) {
  assert(i>=0 && i<m_numberOfModels);
  m_functions[i] = *(static_cast<CartesianFunction *>(e));
}

}
