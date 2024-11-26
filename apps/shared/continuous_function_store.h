#ifndef SHARED_CONTINUOUS_FUNCTION_STORE_H
#define SHARED_CONTINUOUS_FUNCTION_STORE_H

#include "continuous_function.h"
#include "function_store.h"

namespace Shared {

class ContinuousFunctionStore : public FunctionStore {
 public:
  // Very large limit, so that records id in name can't exceed two chars.
  constexpr static int k_maxNumberOfModels = 100;
  constexpr static int k_maxNumberOfMemoizedModels = 10;
  bool memoizationOverflows() const {
    return numberOfModels() > maxNumberOfMemoizedModels();
  }

  static bool IsFunctionAreaCompatible(ExpressionModelHandle* model,
                                       void* context) {
    return IsFunctionActive(model, context) &&
           static_cast<ContinuousFunction*>(model)->canComputeArea();
  }

  ContinuousFunctionStore() : FunctionStore() {}
  int numberOfActiveFunctionsInTable() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveInTable, nullptr);
  }
  bool displaysOnlyCartesianFunctions(int* nbActiveFunctions = nullptr) const;
  bool displaysFunctionsToNormalize(int* nbActiveFunctions = nullptr) const;
  int numberOfActiveFunctionsInTableOfSymbolType(
      ContinuousFunctionProperties::SymbolType symbolType) const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveInTableOfSymbolType,
                                        &symbolType);
  }
  typedef bool (ContinuousFunctionProperties::*HasProperty)() const;
  int numberOfActiveFunctions() const override;
  int numberOfActiveFunctionsWithProperty(HasProperty propertyFunction) const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveAndHasProperty,
                                        &propertyFunction);
  }
  int numberOfIntersectableFunctions() const {
    return numberOfModelsSatisfyingTest(&IsFunctionIntersectable, nullptr);
  }
  int numberOfAreaCompatibleFunctions() const {
    return numberOfModelsSatisfyingTest(&IsFunctionAreaCompatible, nullptr);
  }
  Ion::Storage::Record activeRecordInTableAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &IsFunctionActiveInTable, nullptr);
  }
  Ion::Storage::Record activeRecordOfSymbolTypeInTableAtIndex(
      ContinuousFunctionProperties::SymbolType symbolType, int i) const {
    return recordSatisfyingTestAtIndex(i, &IsFunctionActiveInTableOfSymbolType,
                                       &symbolType);
  }
  ExpiringPointer<ContinuousFunction> modelForRecord(
      Ion::Storage::Record record) const {
    return ExpiringPointer<ContinuousFunction>(
        static_cast<ContinuousFunction*>(privateModelForRecord(record)));
  }
  ExpiringPointer<const ContinuousFunction> constModelForRecord(
      Ion::Storage::Record record) const {
    return ExpiringPointer<const ContinuousFunction>(
        static_cast<const ContinuousFunction*>(privateModelForRecord(record)));
  }
  void setCachesContainer(CachesContainer* container) {
    m_cachesContainer = container;
  }
  ContinuousFunctionCache* cacheAtIndex(int i) const {
    return (m_cachesContainer && i < CachesContainer::k_numberOfAvailableCaches)
               ? m_cachesContainer->cacheAtIndex(i)
               : nullptr;
  }

  KDColor colorForNewModel() const;
  ContinuousFunction newModel(const char* name,
                              Ion::Storage::Record::ErrorStatus* error);
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  int maxNumberOfModels() const override { return k_maxNumberOfModels; }

 private:
  static bool IsFunctionActiveInTable(ExpressionModelHandle* model,
                                      void* context) {
    // An active function must be defined
    return IsFunctionActive(model, context) &&
           static_cast<ContinuousFunction*>(model)->isActiveInTable();
  }
  static bool IsFunctionActiveAndHasProperty(ExpressionModelHandle* model,
                                             void* context) {
    HasProperty propertyFunction = *static_cast<HasProperty*>(context);
    return IsFunctionActive(model, context) &&
           (static_cast<ContinuousFunction*>(model)->properties().*
            propertyFunction)();
  }
  static bool IsFunctionActiveInTableOfSymbolType(ExpressionModelHandle* model,
                                                  void* context) {
    ContinuousFunctionProperties::SymbolType symbolType =
        *static_cast<ContinuousFunctionProperties::SymbolType*>(context);
    return IsFunctionActiveInTable(model, context) &&
           symbolType == static_cast<ContinuousFunction*>(model)
                             ->properties()
                             .symbolType();
  }
  static bool IsFunctionIntersectable(ExpressionModelHandle* model,
                                      void* context) {
    return static_cast<ContinuousFunction*>(model)
        ->shouldDisplayIntersections();
  }
  int maxNumberOfMemoizedModels() const override {
    return k_maxNumberOfMemoizedModels;
  }
  const char* modelExtension() const override {
    return Ion::Storage::functionExtension;
  }
  ExpressionModelHandle* setMemoizedModelAtIndex(
      int cacheIndex, Ion::Storage::Record record) const override;
  ExpressionModelHandle* memoizedModelAtIndex(int cacheIndex) const override;

  void fillLastFreeColors(bool* colorIsFree) const;

  mutable uint32_t m_storageCheckSum;
  mutable int m_memoizedNumberOfActiveFunctions;
  mutable ContinuousFunction m_functions[k_maxNumberOfMemoizedModels];
  CachesContainer* m_cachesContainer;
};

}  // namespace Shared

#endif
