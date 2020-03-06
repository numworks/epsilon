#include "continuous_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

using namespace Shared;

namespace Graph {

bool ContinuousFunctionStore::displaysNonCartesianFunctions(int * nbActiveFunctions) const {
  int nbOfActiveFunctions = numberOfActiveFunctions();
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = nbOfActiveFunctions;
  }
  return numberOfActiveFunctionsOfType(ContinuousFunction::PlotType::Cartesian) != nbOfActiveFunctions;
}

Ion::Storage::Record::ErrorStatus ContinuousFunctionStore::addEmptyModel() {
  Ion::Storage::Record::ErrorStatus error;
  ContinuousFunction newModel = ContinuousFunction::NewModel(&error);
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
