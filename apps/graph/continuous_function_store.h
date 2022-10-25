#ifndef GRAPH_CONTINUOUS_FUNCTION_STORE_H
#define GRAPH_CONTINUOUS_FUNCTION_STORE_H

#include "../shared/function_store.h"
#include "../shared/continuous_function.h"

namespace Graph {

class ContinuousFunctionStore : public Shared::FunctionStore {
public:
  // Very large limit, so that records id in name can't exceed two chars.
  constexpr static int k_maxNumberOfModels = 100;
  constexpr static int k_maxNumberOfMemoizedModels = 10;
  static bool IsFunctionActiveAndDerivable(Shared::ExpressionModelHandle * model, void * context) {
    return IsFunctionActive(model, context) && static_cast<Shared::ContinuousFunction *>(model)->canDisplayDerivative();
  }

  ContinuousFunctionStore() : FunctionStore() {}
  int numberOfActiveFunctionsInTable() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveInTable, nullptr);
  }
  bool displaysNonCartesianFunctions(int * nbActiveFunctions = nullptr) const;
  bool displaysFunctionsToNormalize(int * nbActiveFunctions = nullptr) const;
  int numberOfActiveFunctionsOfSymbolType(Shared::FunctionType::SymbolType symbolType) const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveOfSymbolType, &symbolType);
  }
  typedef bool (Shared::ContinuousFunctionProperties::*HasProperty)() const;
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
  Ion::Storage::Record activeRecordOfSymbolTypeAtIndex(Shared::FunctionType::SymbolType symbolType, int i) const {
    return recordSatisfyingTestAtIndex(i, &IsFunctionActiveOfSymbolType, &symbolType);
  }
  Shared::ExpiringPointer<Shared::ContinuousFunction> modelForRecord(Ion::Storage::Record record) const {
    return Shared::ExpiringPointer<Shared::ContinuousFunction>(static_cast<Shared::ContinuousFunction *>(privateModelForRecord(record)));
  }
  Shared::ContinuousFunctionCache * cacheAtIndex(int i) const { return (i < Shared::ContinuousFunctionCache::k_numberOfAvailableCaches) ? m_functionCaches + i : nullptr; }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  int maxNumberOfModels() const override { return k_maxNumberOfModels; }

private:
  static bool IsFunctionActiveInTable(Shared::ExpressionModelHandle * model, void * context) {
    // An active function must be defined
    return IsFunctionActive(model, context) && static_cast<Shared::ContinuousFunction *>(model)->isActiveInTable();
  }
  static bool IsFunctionActiveOfSymbolType(Shared::ExpressionModelHandle * model, void * context) {
    Shared::FunctionType::SymbolType symbolType = *static_cast<Shared::FunctionType::SymbolType *>(context);
    return IsFunctionActiveInTable(model, context) && symbolType == static_cast<Shared::ContinuousFunction *>(model)->properties().symbolType();
  }
  static bool IsFunctionIntersectable(Shared::ExpressionModelHandle * model, void * context) {
    return static_cast<Shared::ContinuousFunction *>(model)->isIntersectable();
  }
  static bool IsFunctionActiveAndHasProperty(Shared::ExpressionModelHandle * model, void * context) {
    HasProperty propertyFunction = *static_cast<HasProperty *>(context);
    return IsFunctionActiveInTable(model, context) && (static_cast<Shared::ContinuousFunction *>(model)->properties().*propertyFunction)();
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
