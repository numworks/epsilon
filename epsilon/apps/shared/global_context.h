#pragma once

#include <assert.h>
#include <ion/storage/file_system.h>
#include <omg/global_box.h>
#include <poincare/variable_store.h>

#include <array>

#include "continuous_function_store.h"
#include "sequence_cache.h"
#include "sequence_context.h"
#include "sequence_store.h"

namespace Shared {

class GlobalContext final : public Poincare::VariableStore {
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

  GlobalContext() : m_sequenceContext(this, s_sequenceStore){};

  ~GlobalContext() {
    // Destroy all static cache and stores
    s_sequenceStore->removeAll();
    s_sequenceCache->resetCache();
    s_continuousFunctionStore->removeAll();
  }

  /* Expression for symbol
   * The expression recorded in global context is already an expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const override;
  bool setExpressionForUserNamed(
      const Poincare::Internal::Tree* expression,
      const Poincare::Internal::Tree* symbol) override;
  static OMG::GlobalBox<SequenceStore> s_sequenceStore;
  static OMG::GlobalBox<SequenceCache> s_sequenceCache;
  static OMG::GlobalBox<ContinuousFunctionStore> s_continuousFunctionStore;
  void storageDidChangeForRecord(const Ion::Storage::Record record);
  SequenceContext* sequenceContext() { return &m_sequenceContext; }
  const SequenceContext* sequenceContext() const { return &m_sequenceContext; }
  void tidyStores();
  void prepareForNewApp();
  // Reset all functions and sequences
  void reset();
  /* Reset all store entries (functions, sequences, expressions, matrices,
   * lists, regressions and parametric components) */
  void resetAll();

 private:
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

class GlobalContextAccessor {
 public:
  // const access
  static inline const Shared::GlobalContext& Context() {
    return *s_globalContext.get();
  }
  // mutable access
  static inline Shared::GlobalContext& Store() {
    return *s_globalContext.get();
  }

  static inline void Init() { s_globalContext.init(); }

 private:
  static inline OMG::GlobalBox<Shared::GlobalContext> s_globalContext;
};

}  // namespace Shared
