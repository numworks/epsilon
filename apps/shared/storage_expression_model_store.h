#ifndef SHARED_STORAGE_EXPRESSION_MODEL_STORE_H
#define SHARED_STORAGE_EXPRESSION_MODEL_STORE_H

#include "storage_expression_model.h"
#include <ion/storage.h>
#include <assert.h>

namespace Shared {

// StorageExpressionModelStore is a handle to Ion::Storage::sharedStorage()

class StorageExpressionModelStore {
  // TODO find better name (once we remove ExpressionModelStore?)
public:
  // Getters
  virtual const char * modelExtension() const = 0;
  int numberOfModels() const;
  int numberOfDefinedModels() const;
  StorageExpressionModel modelAtIndex(int i) const;
  StorageExpressionModel definedModelAtIndex(int i) const;

  // Add and Remove
  virtual Ion::Storage::Record::ErrorStatus addEmptyModel() = 0;
  void removeModel(StorageExpressionModel * f) {
    assert(f != nullptr);
    f->destroy();
  }
  virtual void removeAll();

  // Other
  void tidy();
};

}

#endif
