#ifndef SHARED_EXPRESSION_MODEL_STORE_H
#define SHARED_EXPRESSION_MODEL_STORE_H

#include "expression_model_handle.h"
#include "expiring_pointer.h"
#include <ion/storage.h>
#include <assert.h>

namespace Shared {

// ExpressionModelStore is a handle to Ion::Storage::sharedStorage()

class ExpressionModelStore {
  // TODO find better name (once we remove ExpressionModelStore?)
public:
  ExpressionModelStore();

  // Getters
  // By default, the number of models is not bounded
  virtual int maxNumberOfModels() const { return -1; }
  int numberOfModels() const;
  Ion::Storage::Record recordAtIndex(int i) const;
  int numberOfDefinedModels() const {
    return numberOfModelsSatisfyingTest(&isModelDefined, nullptr);
  }
  Ion::Storage::Record definedRecordAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &isModelDefined, nullptr);
  }
  ExpiringPointer<ExpressionModelHandle> modelForRecord(Ion::Storage::Record record) const { return ExpiringPointer<ExpressionModelHandle>(privateModelForRecord(record)); }

  // Add and Remove
  virtual Ion::Storage::Record::ErrorStatus addEmptyModel() = 0;
  void removeAll();
  void removeModel(Ion::Storage::Record record);

  // Other
  virtual void tidy();
  void storageDidChangeForRecord(const Ion::Storage::Record record) const { resetMemoizedModelsExceptRecord(record); }
protected:
  constexpr static int k_maxNumberOfMemoizedModels = 10;
  int maxNumberOfMemoizedModels() const { return maxNumberOfModels() < 0 ? k_maxNumberOfMemoizedModels : maxNumberOfModels(); }
  typedef bool (*ModelTest)(ExpressionModelHandle * model, void * context);
  int numberOfModelsSatisfyingTest(ModelTest test, void * context) const;
  Ion::Storage::Record recordSatisfyingTestAtIndex(int i, ModelTest test, void * context) const;
  static bool isModelDefined(ExpressionModelHandle * model, void * context) {
    return model->isDefined();
  }
  ExpressionModelHandle * privateModelForRecord(Ion::Storage::Record record) const;
private:
  void resetMemoizedModelsExceptRecord(const Ion::Storage::Record record = Ion::Storage::Record()) const;
  virtual ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record) const = 0;
  virtual ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const = 0;
  virtual const char * modelExtension() const = 0;
  /* Memoization of k_maxNumberOfMemoizedModels. When the required model is not
   * present, we override the m_oldestMemoizedIndex model. This actually
   * overrides the oldest memoized model because models are all reset at the
   * same time. Otherwise, we should use a queue to decide which was the last
   * memoized model. */
  mutable int m_oldestMemoizedIndex;
};

}

#endif
