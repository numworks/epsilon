#include "cartesian_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

using namespace Shared;

namespace Graph {

bool CartesianFunctionStore::displaysNonCartesianFunctions(int * nbActiveFunctions) const {
  int nbOfActiveFunctions = numberOfActiveFunctions();
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = nbOfActiveFunctions;
  }
  return numberOfActiveFunctionsOfType(CartesianFunction::PlotType::Cartesian) != nbOfActiveFunctions;
}

int CartesianFunctionStore::numberOfActiveFunctionsOfType(CartesianFunction::PlotType plotType) const {
  int count = 0;
  for (int i = 0; i < numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = activeRecordAtIndex(i);
    ExpiringPointer<CartesianFunction> function = modelForRecord(record);
    count += (plotType == function->plotType());
  }
  return count;
}

Ion::Storage::Record CartesianFunctionStore::activeRecordOfTypeAtIndex(CartesianFunction::PlotType plotType, int index) const {
  int count = 0;
  Ion::Storage::Record record;
  for (int i = 0; i < numberOfActiveFunctions(); i++) {
    record = activeRecordAtIndex(i);
    ExpiringPointer<CartesianFunction> function = modelForRecord(record);
    if (plotType == function->plotType()) {
      if (count == index) {
        break;
      }
      count++;
    }
  }
  return record;
}

Ion::Storage::Record::ErrorStatus CartesianFunctionStore::addEmptyModel() {
  Ion::Storage::Record::ErrorStatus error;
  CartesianFunction newModel = CartesianFunction::NewModel(&error);
  return error;
}

ExpressionModelHandle * CartesianFunctionStore::setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_functions[cacheIndex] = CartesianFunction(record);
  return &m_functions[cacheIndex];
}

ExpressionModelHandle * CartesianFunctionStore::memoizedModelAtIndex(int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_functions[cacheIndex];
}

}
