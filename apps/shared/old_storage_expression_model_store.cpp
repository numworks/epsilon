#include "storage_expression_model_store.h"
#include "function.h"
#include <assert.h>

namespace Shared {

StorageExpressionModel StorageExpressionModelStore::addEmptyModel() {
  int modelsCount = numberOfModels();
  StorageExpressionModel newModel = emptyModel();
  setModelAtIndex(newModel, modelsCount);
  return newModel;
}

void StorageExpressionModelStore::removeModel(StorageExpressionModel f) {
  f.destroy();
}

void StorageExpressionModelStore::removeAll() {
  while (!modelAtIndex(0).isEmpty()) {
    modelAtIndex(0).destroy();
  }
}

StorageExpressionModel StorageExpressionModelStore::definedModelAtIndex(int i) {
  assert(i >= 0 && i < numberOfModels());
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

int StorageExpressionModelStore::numberOfDefinedModels() {
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

void StorageExpressionModelStore::tidy() {
  int modelsCount = numberOfModels();
  for (int i = 0; i < modelsCount; i++) {
    modelAtIndex(i).tidy();
  }
}

}
