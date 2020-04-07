#ifndef SHARED_FUNCTION_STORE_H
#define SHARED_FUNCTION_STORE_H

#include "function.h"
#include "expression_model_store.h"
#include <stdint.h>

namespace Shared {

// FunctionStore stores functions and gives them a color.

class FunctionStore : public ExpressionModelStore {
public:
  FunctionStore() : ExpressionModelStore() {}
  uint32_t storeChecksum();
  uint32_t storeChecksumAtIndex(int i);
  int numberOfActiveFunctions() const {
    return numberOfModelsSatisfyingTest(&isFunctionActive, nullptr);
  }
  Ion::Storage::Record activeRecordAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &isFunctionActive, nullptr);
  }
  ExpiringPointer<Function> modelForRecord(Ion::Storage::Record record) const { return ExpiringPointer<Function>(static_cast<Function *>(privateModelForRecord(record))); }
protected:
  static bool isFunctionActive(ExpressionModelHandle * model, void * context) {
    // An active function must be defined
    return isModelDefined(model, context) && static_cast<Function *>(model)->isActive();
  }
};

}

#endif
