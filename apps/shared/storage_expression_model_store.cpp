#include "storage_expression_model_store.h"

namespace Shared {

int StorageExpressionModelStore::numberOfModels() const {
  return Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(modelExtension());
}

int StorageExpressionModelStore::numberOfDefinedModels() const {
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

StorageExpressionModel StorageExpressionModelStore::modelAtIndex(int i) const {
  return StorageExpressionModel(Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(modelExtension(), i));
}

StorageExpressionModel StorageExpressionModelStore::definedModelAtIndex(int i) const {
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

void StorageExpressionModelStore::removeAll() {
  Ion::Storage::sharedStorage()->destroyRecordsWithExtension(modelExtension());
}

void StorageExpressionModelStore::tidy() {
  int modelsCount = numberOfModels();
  for (int i = 0; i < modelsCount; i++) {
    modelAtIndex(i).tidy();
  }
}

}
