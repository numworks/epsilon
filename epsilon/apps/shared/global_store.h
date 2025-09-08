#pragma once

#include <assert.h>
#include <ion/storage/file_system.h>
#include <omg/global_box.h>
#include <poincare/symbol_store_interface.h>

#include <array>

#include "continuous_function_store.h"
#include "sequence_cache.h"
#include "sequence_context.h"
#include "sequence_store.h"

namespace Shared {

class GlobalStore final : public Poincare::SymbolStoreInterface {
  friend class GlobalContextAccessor;
  friend class OMG::GlobalBox<GlobalStore>;

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

  static void Init();

  // Storage information
  static bool UserNameIsFree(const char* baseName);

  static Poincare::Layout LayoutForRecord(Ion::Storage::Record record);

  // Destroy records
  static void DestroyRecordsBaseNamedWithoutExtension(const char* baseName,
                                                      const char* extension);
  static void DeleteParametricComponentsWithBaseName(char* baseName,
                                                     size_t baseNameLength,
                                                     size_t bufferSize);
  static void DeleteParametricComponentsOfRecord(Ion::Storage::Record record);
  static void StoreParametricComponentsOfRecord(Ion::Storage::Record record);

  /* Expression for symbol
   * The expression recorded in global context is already an expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const override;
  bool setExpressionForUserNamed(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* symbol) override;
  void storageDidChangeForRecord(const Ion::Storage::Record record);
  SequenceContext& sequenceContext() { return m_sequenceContext; }
  const SequenceContext& sequenceContext() const { return m_sequenceContext; }
  void tidyStores();
  void prepareForNewApp();
  // Reset all functions and sequences
  void reset();
  /* Reset all store entries (functions, sequences, expressions, matrices,
   * lists, regressions and parametric components) */
  void resetAll();

 private:
  /* REFACTOR: SequenceStore, SequenceCache and SequenceContext should be
   * grouped in a single interface. SequenceContext could be the const version
   * of SequenceStore (like ContinuousFonctionContext and
   * ContinuousFonctionStore). */
  static OMG::GlobalBox<SequenceStore> s_sequenceStore;
  static OMG::GlobalBox<SequenceCache> s_sequenceCache;
  static OMG::GlobalBox<ContinuousFunctionStore> s_continuousFunctionStore;

  /* GlobalStore is a singleton. The unique instance can be accessed through
   * GlobalContextAccessor */
  GlobalStore() : m_sequenceContext(this, s_sequenceStore){};

  // Expression getters
  const Poincare::Internal::Tree* expressionForUserNamed(
      const Poincare::Internal::Tree* symbol) const override;
  const Poincare::Internal::Tree* expressionForSymbolAndRecord(
      const Poincare::Internal::Tree* symbol, Ion::Storage::Record r) const;
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

  SequenceContext m_sequenceContext;
};

using GlobalContext = const GlobalStore;

class GlobalContextAccessor {
 public:
  // const access
  static inline const Shared::GlobalContext& Context() {
    return *s_globalStore.get();
  }
  static inline const Shared::SequenceContext& SequenceContext() {
    return Context().sequenceContext();
  }

  static inline const Shared::ContinuousFunctionContext&
  ContinuousFunctionContext() {
    return *GlobalStore::s_continuousFunctionStore.get();
  }

  // mutable access
  static inline Shared::GlobalStore& Store() { return *s_globalStore.get(); }

  static inline Shared::ContinuousFunctionStore& ContinuousFunctionStore() {
    return *GlobalStore::s_continuousFunctionStore.get();
  }

  static inline Shared::SequenceStore& SequenceStore() {
    return *GlobalStore::s_sequenceStore.get();
  }

  static inline Shared::SequenceCache& SequenceCache() {
    return *GlobalStore::s_sequenceCache.get();
  }

  static inline void Init() { s_globalStore.init(); }

 private:
  static inline OMG::GlobalBox<Shared::GlobalStore> s_globalStore;
};

}  // namespace Shared
