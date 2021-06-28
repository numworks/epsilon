#ifndef GRAPH_FUNCTION_STORE_H
#define GRAPH_FUNCTION_STORE_H

#include "../shared/expression_model_store.h"
#include "new_function.h"
#include <stdint.h>

namespace Graph {

// ContinuousFunctionStore stores functions and gives them a color.

class ContinuousFunctionStore : public Shared::ExpressionModelStore {
public:
  ContinuousFunctionStore() : ExpressionModelStore() {}
  uint32_t storeChecksum();
  int numberOfActiveFunctions() const {
    return numberOfModelsSatisfyingTest(&isFunctionActive, nullptr);
  }
  Ion::Storage::Record activeRecordAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &isFunctionActive, nullptr);
  }
  // TODO Hugo
  bool displaysNonCartesianFunctions(int * nbActiveFunctions = nullptr) const;
  int numberOfActiveFunctionsOfType(NewFunction::PlotType plotType) const {
    return numberOfModelsSatisfyingTest(&isFunctionActiveOfType, &plotType);
  }
  Ion::Storage::Record activeRecordOfTypeAtIndex(NewFunction::PlotType plotType, int i) const {
    return recordSatisfyingTestAtIndex(i, &isFunctionActiveOfType, &plotType);
  }
  Shared::ExpiringPointer<NewFunction> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<NewFunction>(static_cast<NewFunction *>(privateModelForRecord(record))); }
  // TODO Hugo : Handle cache
  // Shared::ContinuousFunctionCache * cacheAtIndex(int i) const { return (i < Shared::ContinuousFunctionCache::k_numberOfAvailableCaches) ? m_functionCaches + i : nullptr; }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;

private:
  static bool isFunctionActive(Shared::ExpressionModelHandle * model, void * context) {
    // An active function must be defined
    return isModelDefined(model, context) && static_cast<NewFunction *>(model)->isActive();
  }
  // TODO Hugo factorize things (isFunctionActive ?)
  const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  // const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  Shared::ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  static bool isFunctionActiveOfType(Shared::ExpressionModelHandle * model, void * context) {
    NewFunction::PlotType plotType = *static_cast<NewFunction::PlotType *>(context);
    return isFunctionActive(model, context) && plotType == static_cast<NewFunction *>(model)->plotType();
  }
  mutable NewFunction m_functions[k_maxNumberOfMemoizedModels];
  // TODO Hugo : Handle cache
  // mutable Shared::ContinuousFunctionCache m_functionCaches[Shared::ContinuousFunctionCache::k_numberOfAvailableCaches];
};

}

#endif
