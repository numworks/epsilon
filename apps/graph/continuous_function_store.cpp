#include "continuous_function_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

using namespace Shared;

namespace Graph {

int ContinuousFunctionStore::numberOfActiveFunctionsOfType(ContinuousFunction::PlotType plotType) const {
  int count = 0;
  for (int i = 0; i < numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = activeRecordAtIndex(i);
    ExpiringPointer<ContinuousFunction> function = modelForRecord(record);
    count += (plotType == function->plotType());
  }
  return count;
}

Ion::Storage::Record ContinuousFunctionStore::activeRecordOfTypeAtIndex(ContinuousFunction::PlotType plotType, int index) const {
  int count = 0;
  Ion::Storage::Record record;
  for (int i = 0; i < numberOfActiveFunctions(); i++) {
    record = activeRecordAtIndex(i);
    ExpiringPointer<ContinuousFunction> function = modelForRecord(record);
    if (plotType == function->plotType()) {
      if (count == index) {
        break;
      }
      count++;
    }
  }
  return record;
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
