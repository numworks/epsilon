#ifndef SHARED_STORAGE_EXPRESSION_MODEL_STORE_H
#define SHARED_STORAGE_EXPRESSION_MODEL_STORE_H

#include <ion/storage.h>
#include "storage_expression_model.h"
#include <stdint.h>
#include <assert.h>

namespace Shared {

// StorageExpressionModelStore is a handle to Ion::sharedStorage

template<class T>
class StorageExpressionModelStore {
  // TODO find better name (once we remove ExpressionModelStore?)
public:
  // Getters
  int numberOfModels() const {
    return Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(T::Extension());
  }
  T modelAtIndex(int i) const {
    return T(Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(T::Extension(), i));
  }
  int numberOfDefinedModels() {
    int result = 0;
    int i = 0;
    while (!modelAtIndex(i).isEmpty()) {
      if (modelAtIndex(i).isDefined()) {
        result++;
      }
      i++;
    }
    return result;
  }

  T definedModelAtIndex(int i) {
    assert(i >= 0 && i < numberOfDefinedModels());
    int index = 0;
    int currentModelIndex = 0;
    while (true) {
      assert(currentModelIndex < numberOfModels());
      if (modelAtIndex(currentModelIndex).isDefined()) {
        if (i == index) {
          return modelAtIndex(currentModelIndex);
        }
        index++;
      }
      currentModelIndex++;
    }
  }

  // Add and Remove
  T addEmptyModel() {
    char nameBuffer[100];
    T::DefaultName(nameBuffer, 100);
    // TODO: create the record data (color, isActive and expression)
    // T::EmptyModelData
    Ion::Storage::sharedStorage()->createRecordWithFullName(nameBuffer, nullptr, 0);
    return T::NewModel(Ion::Storage::sharedStorage()->recordNamed(nameBuffer));
  }
  void removeModel(T f) {
    f.destroy();
  }
  virtual void removeAll() {
    Ion::Storage::sharedStorage()->destroyRecordsWithExtension(T::Extension());
  }

  // Other
  void tidy() {
    int modelsCount = numberOfModels();
    for (int i = 0; i < modelsCount; i++) {
      modelAtIndex(i).tidy();
    }
  }
};

}

#endif
