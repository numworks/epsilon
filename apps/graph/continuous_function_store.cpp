#include "continuous_function_store.h"

namespace Graph {

uint32_t ContinuousFunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

bool ContinuousFunctionStore::displaysNonCartesianFunctions(int * nbActiveFunctions) const {
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = numberOfActiveFunctions();
  }
  return numberOfActiveFunctionsOfType(NewFunction::PlotType::Polar) + numberOfActiveFunctionsOfType(NewFunction::PlotType::Parametric) == 0;
}

Ion::Storage::Record::ErrorStatus ContinuousFunctionStore::addEmptyModel() {
  char name[3] = {'?', '?', 0}; // name is going to be e0 or e1 or ... e5
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
  NewFunction newModel = NewFunction::NewModel(&error, name);
  return error;
}

Shared::ExpressionModelHandle * ContinuousFunctionStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_functions[cacheIndex] = NewFunction(record);
  return &m_functions[cacheIndex];
}

Shared::ExpressionModelHandle * ContinuousFunctionStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_functions[cacheIndex];
}

}
