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
  StorageExpressionModelStore();

  // Getters
  int numberOfModels() const;
  int numberOfDefinedModels() const;
  virtual StorageExpressionModel * modelAtIndex(int i) const;
  virtual StorageExpressionModel * definedModelAtIndex(int i) const;

  // Add and Remove
  virtual Ion::Storage::Record::ErrorStatus addEmptyModel() = 0;
  void removeAll();
  void removeModel(StorageExpressionModel * f);

  // Other
  void tidy();
protected:
  constexpr static int k_maxNumberOfMemoizedModels = 5;
private:
  virtual const char * modelExtension() const = 0;
  /* Memoization of k_maxNumberOfMemoizedModels consecutive models:
   * stores memoize k_maxNumberOfMemoizedModels consecutive models from the
   * m_firstMemoizedModelIndex. */
  mutable int m_firstMemoizedModelIndex;
  mutable uint32_t m_storageChecksum;
  virtual void setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record) const = 0;
  virtual void moveMemoizedModel(int previousIndex, int nextIndex) const = 0;
  virtual StorageExpressionModel * memoizedModelAtIndex(int cacheIndex) const = 0;
};

}

#endif
