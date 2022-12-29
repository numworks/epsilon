#ifndef SHARED_CONTINUOUS_FUNCTION_STORE_H
#define SHARED_CONTINUOUS_FUNCTION_STORE_H

#include "function_store.h"
#include "continuous_function.h"

namespace Shared {

class ContinuousFunctionStore : public FunctionStore {
public:
  // Very large limit, so that records id in name can't exceed two chars.
  constexpr static int k_maxNumberOfModels = 100;
  constexpr static int k_maxNumberOfMemoizedModels = 10;
  bool memoizationIsFull() const { return numberOfModels() >= maxNumberOfMemoizedModels(); }

  static bool IsFunctionActiveAndDerivable(ExpressionModelHandle * model, void * context) {
    return IsFunctionActive(model, context) && static_cast<ContinuousFunction *>(model)->canDisplayDerivative();
  }

  ContinuousFunctionStore() : FunctionStore() {}
  int numberOfActiveFunctionsInTable() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveInTable, nullptr);
  }
  bool displaysNonCartesianFunctions(int * nbActiveFunctions = nullptr) const;
  bool displaysFunctionsToNormalize(int * nbActiveFunctions = nullptr) const;
  int numberOfActiveFunctionsInTableOfSymbolType(ContinuousFunctionProperties::SymbolType symbolType) const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveInTableOfSymbolType, &symbolType);
  }
  typedef bool (ContinuousFunctionProperties::*HasProperty)() const;
  int numberOfActiveFunctionsWithProperty(HasProperty propertyFunction) const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveAndHasProperty, &propertyFunction);
  }
  int numberOfIntersectableFunctions() const {
    return numberOfModelsSatisfyingTest(&IsFunctionIntersectable, nullptr);
  }
  int numberOfActiveDerivableFunctions() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveAndDerivable, nullptr);
  }
  Ion::Storage::Record activeRecordInTableAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &IsFunctionActiveInTable, nullptr);
  }
  Ion::Storage::Record activeRecordOfSymbolTypeInTableAtIndex(ContinuousFunctionProperties::SymbolType symbolType, int i) const {
    return recordSatisfyingTestAtIndex(i, &IsFunctionActiveInTableOfSymbolType, &symbolType);
  }
  ExpiringPointer<ContinuousFunction> modelForRecord(Ion::Storage::Record record) const {
    return ExpiringPointer<ContinuousFunction>(static_cast<ContinuousFunction *>(privateModelForRecord(record)));
  }
  KDColor colorForRecord(Ion::Storage::Record record) const override { return modelForRecord(record)->color(); }
  ContinuousFunctionCache * cacheAtIndex(int i) const { return (i < ContinuousFunctionCache::k_numberOfAvailableCaches) ? m_functionCaches + i : nullptr; }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  int maxNumberOfModels() const override { return k_maxNumberOfModels; }

private:
  static bool IsFunctionActiveInTable(ExpressionModelHandle * model, void * context) {
    // An active function must be defined
    return IsFunctionActive(model, context) && static_cast<ContinuousFunction *>(model)->isActiveInTable();
  }
  static bool IsFunctionActiveAndHasProperty(ExpressionModelHandle * model, void * context) {
    HasProperty propertyFunction = *static_cast<HasProperty *>(context);
    return IsFunctionActive(model, context) && (static_cast<ContinuousFunction *>(model)->properties().*propertyFunction)();
  }
  static bool IsFunctionActiveInTableOfSymbolType(ExpressionModelHandle * model, void * context) {
    ContinuousFunctionProperties::SymbolType symbolType = *static_cast<ContinuousFunctionProperties::SymbolType *>(context);
    return IsFunctionActiveInTable(model, context) && symbolType == static_cast<ContinuousFunction *>(model)->properties().symbolType();
  }
  static bool IsFunctionIntersectable(ExpressionModelHandle * model, void * context) {
    return static_cast<ContinuousFunction *>(model)->shouldDisplayIntersections();
  }
  int maxNumberOfMemoizedModels() const override { return k_maxNumberOfMemoizedModels; }
  const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;

  mutable ContinuousFunction m_functions[k_maxNumberOfMemoizedModels];
  mutable ContinuousFunctionCache m_functionCaches[ContinuousFunctionCache::k_numberOfAvailableCaches];

};

}

#endif
