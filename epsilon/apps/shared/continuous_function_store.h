#pragma once

#include <omg/global_box.h>

#include "continuous_function.h"
#include "function_store.h"

namespace Shared {

class ContinuousFunctionStore : public FunctionStore {
  friend class OMG::GlobalBox<ContinuousFunctionStore>;
  friend class ContinuousFunctionStoreTestBuilder;

 public:
  // Very large limit, so that records id in name can't exceed two chars.
  constexpr static int k_maxNumberOfModels = 100;
  constexpr static int k_maxNumberOfMemoizedModels = 10;
  bool memoizationOverflows() const {
    return numberOfModels() > maxNumberOfMemoizedModels();
  }

  static bool IsFunctionAreaCompatible(const ExpressionModelHandle* model,
                                       const void* context) {
    return IsFunctionActive(model, context) &&
           static_cast<const ContinuousFunction*>(model)->canComputeArea();
  }

  int numberOfActiveFunctionsInTable() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveInTable, nullptr);
  }
  // Remove all functions and memoized functions.
  void removeAll() override;
  bool displaysOnlyCartesianFunctions(int* nbActiveFunctions = nullptr) const;
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
  int numberOfInequalityFunctions() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActiveInequality, nullptr);
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
  OMG::ExpiringPointer<ContinuousFunction> modelForRecord(
      Ion::Storage::Record record) {
    return OMG::ExpiringPointer<ContinuousFunction>(
        static_cast<ContinuousFunction*>(privateModelForRecord(record)));
  }
  OMG::ExpiringPointer<const ContinuousFunction> modelForRecord(
      Ion::Storage::Record record) const {
    return OMG::ExpiringPointer<const ContinuousFunction>(
        static_cast<const ContinuousFunction*>(privateModelForRecord(record)));
  }
  // Tidy the cache of all memoized models
  void tidyCaches() const;
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
  /* ContinuousFunctionStore is a singleton. The unique instance can be accessed
   * through GlobalContextAccessor */
  ContinuousFunctionStore() = default;

  static bool IsFunctionActiveInTable(const ExpressionModelHandle* model,
                                      const void* context) {
    // An active function must be defined
    return IsFunctionActive(model, context) &&
           static_cast<const ContinuousFunction*>(model)->isActiveInTable();
  }
  static bool IsFunctionActiveAndHasProperty(const ExpressionModelHandle* model,
                                             const void* context) {
    HasProperty propertyFunction = *static_cast<const HasProperty*>(context);
    return IsFunctionActive(model, context) &&
           (static_cast<const ContinuousFunction*>(model)->properties().*
            propertyFunction)();
  }
  static bool IsFunctionActiveInTableOfSymbolType(
      const ExpressionModelHandle* model, const void* context) {
    ContinuousFunctionProperties::SymbolType symbolType =
        *static_cast<const ContinuousFunctionProperties::SymbolType*>(context);
    return IsFunctionActiveInTable(model, context) &&
           symbolType == static_cast<const ContinuousFunction*>(model)
                             ->properties()
                             .symbolType();
  }
  static bool IsFunctionActiveInequality(const ExpressionModelHandle* model,
                                         const void* context) {
    return IsFunctionActive(model, context) &&
           !(static_cast<const ContinuousFunction*>(model)
                 ->properties()
                 .isEquality());
  }
  static bool IsFunctionIntersectable(const ExpressionModelHandle* model,
                                      const void* context) {
    return static_cast<const ContinuousFunction*>(model)
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

using ContinuousFunctionContext = const ContinuousFunctionStore;

}  // namespace Shared
