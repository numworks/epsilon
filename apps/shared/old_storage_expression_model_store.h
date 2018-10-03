#ifndef STORAGE_SHARED_EXPRESSION_MODEL_STORE_H
#define STORAGE_SHARED_EXPRESSION_MODEL_STORE_H

#include "storage_expression_model.h"
#include <stdint.h>

namespace Shared {

// StorageExpressionModelStore is a handle to Ion::sharedStorage

class StorageExpressionModelStore {
  // TODO find better name (once we remove ExpressionModelStore?)
public:
  virtual StorageExpressionModel modelAtIndex(int i) = 0;
  StorageExpressionModel addEmptyModel();
  void removeModel(StorageExpressionModel f);
  virtual void removeAll();
  virtual int numberOfModels() const;
  virtual StorageExpressionModel definedModelAtIndex(int i);
  int numberOfDefinedModels();
  virtual void tidy();
protected:
  virtual StorageExpressionModel emptyModel() = 0;
  virtual StorageExpressionModel nullModel() = 0;
  virtual void setModelAtIndex(StorageExpressionModel f, int i) = 0;
};

}

#endif
