#ifndef SHARED_EXPRESSION_MODEL_STORE_H
#define SHARED_EXPRESSION_MODEL_STORE_H

#include <assert.h>
#include <ion/storage/file_system.h>

#include "expiring_pointer.h"
#include "expression_model_handle.h"

namespace Shared {

// ExpressionModelStore is a handle to
// Ion::Storage::FileSystem::sharedFileSystem

class ExpressionModelStore {
  // TODO find better name (once we remove ExpressionModelStore?)
 public:
  ExpressionModelStore();

  // Getters
  virtual const char* modelExtension() const = 0;
  // Override with -1 for an unbounded max number of models.
  virtual int maxNumberOfModels() const { return maxNumberOfMemoizedModels(); }
  int numberOfModels() const;
  Ion::Storage::Record recordAtIndex(int i) const;
  int numberOfDefinedModels() const {
    return numberOfModelsSatisfyingTest(&isModelDefined, nullptr);
  }
  Ion::Storage::Record definedRecordAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &isModelDefined, nullptr);
  }
  ExpiringPointer<ExpressionModelHandle> modelForRecord(
      Ion::Storage::Record record) const {
    return ExpiringPointer<ExpressionModelHandle>(
        privateModelForRecord(record));
  }

  // Add and Remove
  virtual Ion::Storage::Record::ErrorStatus addEmptyModel() = 0;
  void removeAll();
  void removeModel(Ion::Storage::Record record);

  // Other
  virtual void tidyDownstreamPoolFrom(char* treePoolCursor = nullptr);
  void storageDidChangeForRecord(const Ion::Storage::Record record) const {
    resetMemoizedModelsExceptRecord(record);
  }

 protected:
  virtual int maxNumberOfMemoizedModels() const = 0;
  typedef bool (*ModelTest)(ExpressionModelHandle* model, void* context);
  int numberOfModelsSatisfyingTest(ModelTest test, void* context) const;
  Ion::Storage::Record recordSatisfyingTestAtIndex(int i, ModelTest test,
                                                   void* context) const;
  static bool isModelDefined(ExpressionModelHandle* model, void* context) {
    return model->isDefined();
  }
  ExpressionModelHandle* privateModelForRecord(
      Ion::Storage::Record record) const;

 private:
  void resetMemoizedModelsExceptRecord(
      const Ion::Storage::Record record = Ion::Storage::Record()) const;
  virtual ExpressionModelHandle* setMemoizedModelAtIndex(
      int cacheIndex, Ion::Storage::Record) const = 0;
  virtual ExpressionModelHandle* memoizedModelAtIndex(int cacheIndex) const = 0;
  /* When the required model is not present, we override the
   * m_oldestMemoizedIndex model. This actually overrides the oldest memoized
   * model because models are all reset at the same time. Otherwise, we should
   * use a queue to decide which was the last memoized model. */
  mutable int m_oldestMemoizedIndex;
};

}  // namespace Shared

#endif
