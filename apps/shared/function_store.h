#ifndef SHARED_FUNCTION_STORE_H
#define SHARED_FUNCTION_STORE_H

#include "continuous_function.h"
#include "expression_model_store.h"
#include <stdint.h>

namespace Shared {

// FunctionStore stores functions and gives them a color.

class FunctionStore : public ExpressionModelStore {
public:
  FunctionStore() : ExpressionModelStore() {}
  uint32_t storeChecksum();
  int numberOfActiveFunctions() const {
    return numberOfModelsSatisfyingTest(&isFunctionActive, nullptr);
  }
  Ion::Storage::Record activeRecordAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &isFunctionActive, nullptr);
  }
  ExpiringPointer<ContinuousFunction> modelForRecord(Ion::Storage::Record record) const { return ExpiringPointer<ContinuousFunction>(static_cast<ContinuousFunction *>(privateModelForRecord(record))); }
protected:
  static bool isFunctionActive(ExpressionModelHandle * model, void * context) {
    // An active function must be defined
    return isModelDefined(model, context) && static_cast<ContinuousFunction *>(model)->isActive();
  }
};

}

#endif
