#include "continuous_function_store.h"

namespace Shared {

uint32_t ContinuousFunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

bool ContinuousFunctionStore::displaysNonCartesianFunctions(int * nbActiveFunctions) const {
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = numberOfActiveFunctions();
  }
  return numberOfActiveFunctionsOfType(ContinuousFunction::PlotType::Polar) + numberOfActiveFunctionsOfType(ContinuousFunction::PlotType::Parametric) == 0;
}

Ion::Storage::Record::ErrorStatus ContinuousFunctionStore::addEmptyModel() {
  // TODO Hugo : Maybe handle more than 10 ?
  char name[3] = {'?', '?', 0}; // name is going to be ?0 or ?1 or ... ?5
  int currentNumber = 0;
  while (currentNumber < k_maxNumberOfMemoizedModels) {
    name[1] = '0'+currentNumber;
    if (Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(name, modelExtension()).isNull()) {
      break;
    }
    currentNumber++;
  }
  assert(currentNumber < k_maxNumberOfMemoizedModels);
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::RecordDoesNotExist;
  ContinuousFunction newModel = ContinuousFunction::NewModel(&error, name);
  return error;
}

ExpressionModelHandle * ContinuousFunctionStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_functions[cacheIndex] = ContinuousFunction(record);
  return &m_functions[cacheIndex];
}

ExpressionModelHandle * ContinuousFunctionStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_functions[cacheIndex];
}

}
