#pragma once

#include <stdint.h>

#include "expression_model_store.h"
#include "function.h"

namespace Shared {

// FunctionStore stores functions and gives them a color.

class FunctionStore : public ExpressionModelStore {
 public:
  FunctionStore() : ExpressionModelStore() {}
  virtual int numberOfActiveFunctions() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActive, nullptr);
  }
  Ion::Storage::Record activeRecordAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &IsFunctionActive, nullptr);
  }
  int indexOfRecordAmongActiveRecords(Ion::Storage::Record record) const {
    int n = numberOfActiveFunctions();
    for (int i = 0; i < n; i++) {
      if (activeRecordAtIndex(i) == record) {
        return i;
      }
    }
    return -1;
  }
  OMG::ExpiringPointer<Function> modelForRecord(Ion::Storage::Record record) {
    return OMG::ExpiringPointer<Function>(
        static_cast<Function*>(privateModelForRecord(record)));
  }
  OMG::ExpiringPointer<const Function> modelForRecord(
      Ion::Storage::Record record) const {
    return OMG::ExpiringPointer<const Function>(
        static_cast<const Function*>(privateModelForRecord(record)));
  }

 protected:
  static bool IsFunctionActive(const ExpressionModelHandle* model,
                               const void* context) {
    // An active function must be defined
    return isModelDefined(model, context) &&
           static_cast<const Function*>(model)->isActive();
  }
};

using FunctionContext = const FunctionStore;

}  // namespace Shared
