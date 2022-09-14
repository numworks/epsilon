#ifndef GRAPH_CONTINUOUS_FUNCTION_STORE_H
#define GRAPH_CONTINUOUS_FUNCTION_STORE_H

#include "../shared/function_store.h"
#include "../shared/continuous_function.h"

namespace Graph {

class ContinuousFunctionStore : public Shared::FunctionStore {
public:
  ContinuousFunctionStore() : FunctionStore() {}
  int numberOfActiveFunctionsInTable() const {
    return numberOfModelsSatisfyingTest(&isFunctionActiveInTable, nullptr);
  }
  bool displaysNonCartesianFunctions(int * nbActiveFunctions = nullptr) const;
  bool displaysFunctionsToNormalize(int * nbActiveFunctions = nullptr) const;
  int numberOfActiveFunctionsOfSymbolType(Shared::ContinuousFunction::SymbolType symbolType) const {
    return numberOfModelsSatisfyingTest(&isFunctionActiveOfSymbolType, &symbolType);
  }
  int numberOfActiveFunctionsOfType(Shared::ContinuousFunction::PlotType plotType) const {
    return numberOfModelsSatisfyingTest(&isFunctionActiveOfType, &plotType);
  }
  int numberOfIntersectableFunctions() const {
    return numberOfModelsSatisfyingTest(&isFunctionIntersectable, nullptr);
  }
  int numberOfActiveDerivableFunctions() const {
    return numberOfModelsSatisfyingTest(&isFunctionActiveAndDerivable, nullptr);
  }
  Ion::Storage::Record activeRecordInTableAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &isFunctionActiveInTable, nullptr);
  }
  Ion::Storage::Record activeRecordOfSymbolTypeAtIndex(Shared::ContinuousFunction::SymbolType symbolType, int i) const {
    return recordSatisfyingTestAtIndex(i, &isFunctionActiveOfSymbolType, &symbolType);
  }
  Shared::ExpiringPointer<Shared::ContinuousFunction> modelForRecord(Ion::Storage::Record record) const {
    return Shared::ExpiringPointer<Shared::ContinuousFunction>(static_cast<Shared::ContinuousFunction *>(privateModelForRecord(record)));
  }
  Shared::ContinuousFunctionCache * cacheAtIndex(int i) const { return (i < Shared::ContinuousFunctionCache::k_numberOfAvailableCaches) ? m_functionCaches + i : nullptr; }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  int maxNumberOfModels() const override { return k_maxNumberOfModels; }

private:
  // Very large limit, so that records id in name can't exceed two chars.
  constexpr static int k_maxNumberOfModels = 100;
  constexpr static int k_maxNumberOfMemoizedModels = 10;
  static bool isFunctionActiveInTable(Shared::ExpressionModelHandle * model, void * context) {
    // An active function must be defined
    return isFunctionActive(model, context) && static_cast<Shared::ContinuousFunction *>(model)->isActiveInTable();
  }
  static bool isFunctionActiveOfType(Shared::ExpressionModelHandle * model, void * context) {
    Shared::ContinuousFunction::PlotType plotType = *static_cast<Shared::ContinuousFunction::PlotType *>(context);
    return isFunctionActive(model, context) && plotType == static_cast<Shared::ContinuousFunction *>(model)->plotType();
  }
  static bool isFunctionActiveOfSymbolType(Shared::ExpressionModelHandle * model, void * context) {
    Shared::ContinuousFunction::SymbolType symbolType = *static_cast<Shared::ContinuousFunction::SymbolType *>(context);
    return isFunctionActiveInTable(model, context) && symbolType == static_cast<Shared::ContinuousFunction *>(model)->symbolType();
  }
  static bool isFunctionIntersectable(Shared::ExpressionModelHandle * model, void * context) {
    return static_cast<Shared::ContinuousFunction *>(model)->isIntersectable();
  }
  static bool isFunctionActiveAndDerivable(Shared::ExpressionModelHandle * model, void * context) {
    return isFunctionActive(model, context) && static_cast<Shared::ContinuousFunction *>(model)->canDisplayDerivative();
  }
  int maxNumberOfMemoizedModels() const override { return k_maxNumberOfMemoizedModels; }
  const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  Shared::ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;

  mutable Shared::ContinuousFunction m_functions[k_maxNumberOfMemoizedModels];
  mutable Shared::ContinuousFunctionCache m_functionCaches[Shared::ContinuousFunctionCache::k_numberOfAvailableCaches];

};

}

#endif
