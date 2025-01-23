#ifndef APPS_SHARED_GLOBAL_CONTEXT_H
#define APPS_SHARED_GLOBAL_CONTEXT_H

#include <assert.h>
#include <ion/storage/file_system.h>
#include <omg/global_box.h>
#include <poincare/old/context.h>

#include <array>

#include "continuous_function_store.h"
#include "sequence_cache.h"
#include "sequence_context.h"
#include "sequence_store.h"

namespace Shared {

class GlobalContext final : public Poincare::Context {
 public:
  constexpr static const char* k_extensions[] = {
      Ion::Storage::expressionExtension,
      Ion::Storage::matrixExtension,
      Ion::Storage::functionExtension,
      Ion::Storage::listExtension,
      Ion::Storage::sequenceExtension,
      Ion::Storage::regressionExtension,
      Ion::Storage::parametricComponentExtension};
  constexpr static int k_numberOfExtensions = std::size(k_extensions);

  // Storage information
  static bool UserNameIsFree(const char* baseName);

  static const Poincare::Layout LayoutForRecord(Ion::Storage::Record record);

  // Destroy records
  static void DestroyRecordsBaseNamedWithoutExtension(const char* baseName,
                                                      const char* extension);
  static void DeleteParametricComponentsWithBaseName(char* baseName,
                                                     size_t baseNameLength,
                                                     size_t bufferSize);
  static void DeleteParametricComponentsOfRecord(Ion::Storage::Record record);
  static void StoreParametricComponentsOfRecord(Ion::Storage::Record record);

  GlobalContext() : m_sequenceContext(this, s_sequenceStore) {
    // GlobalContext expects to have a single instance at a time
    assert(Poincare::Context::GlobalContext == nullptr);
    Poincare::Context::GlobalContext = this;
  };

  ~GlobalContext() {
    // Destroy all static cache and stores
    s_sequenceStore->removeAll();
    s_sequenceCache->resetCache();
    // TODO : Unit tests expect preservation of s_continuousFunctionStore
    // s_continuousFunctionStore->removeAll();
    Poincare::Context::GlobalContext = nullptr;
  }

  /* Expression for symbol
   * The expression recorded in global context is already an expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) override;
  bool setExpressionForUserNamed(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* symbol) override;
  static OMG::GlobalBox<SequenceStore> s_sequenceStore;
  static OMG::GlobalBox<SequenceCache> s_sequenceCache;
  static OMG::GlobalBox<ContinuousFunctionStore> s_continuousFunctionStore;
  void storageDidChangeForRecord(const Ion::Storage::Record record);
  SequenceContext* sequenceContext() { return &m_sequenceContext; }
#if POINCARE_CONTEXT_TIDY_POOL
  void tidyDownstreamPoolFrom(
      Poincare::PoolObject* treePoolCursor = nullptr) override;
#endif
  void prepareForNewApp();
  void reset();

 private:
  // Expression getters
  const Poincare::Internal::Tree* expressionForUserNamed(
      const Poincare::Internal::Tree* symbol) override;
  const Poincare::Internal::Tree* expressionForSymbolAndRecord(
      const Poincare::Internal::Tree* symbol, Ion::Storage::Record r);
  static const Poincare::Internal::Tree* ExpressionForUserSymbol(
      Ion::Storage::Record r);
  static const Poincare::Internal::Tree* ExpressionForUserFunction(
      Ion::Storage::Record r);
  // Expression setters
  /* This modifies the expression. */
  Ion::Storage::Record::ErrorStatus setExpressionForUserSymbol(
      Poincare::UserExpression& expression, const char* name,
      Ion::Storage::Record previousRecord);
  Ion::Storage::Record::ErrorStatus setExpressionForUserFunction(
      const Poincare::UserExpression& expression,
      const Poincare::UserExpression& symbol,
      Ion::Storage::Record previousRecord);
  // Record getter
  static Ion::Storage::Record UserNamedRecordWithBaseName(const char* name);
  double approximateSequenceAtRank(const char* identifier,
                                   int rank) const override;
  SequenceContext m_sequenceContext;
};

}  // namespace Shared

#endif
