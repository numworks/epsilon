#ifndef APPS_SHARED_GLOBAL_CONTEXT_H
#define APPS_SHARED_GLOBAL_CONTEXT_H

#include <assert.h>
#include <ion/storage/file_system.h>
#include <omg/global_box.h>
#include <poincare/old/context.h>
#include <poincare/old/symbol.h>
#include <poincare/src/expression/sequence_cache.h>

#include <array>

#include "continuous_function_store.h"
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
  static bool SymbolAbstractNameIsFree(const char* baseName);

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

  ~GlobalContext() { Poincare::Context::GlobalContext = nullptr; }

  /* Expression for symbol
   * The expression recorded in global context is already an expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                 int length) override;
  bool setExpressionForSymbolAbstract(
      const Poincare::UserExpression& expression,
      const Poincare::SymbolAbstract& symbol) override;
  static OMG::GlobalBox<SequenceStore> s_sequenceStore;
  static OMG::GlobalBox<Poincare::Internal::SequenceCache> s_sequenceCache;
  static OMG::GlobalBox<ContinuousFunctionStore> s_continuousFunctionStore;
  void storageDidChangeForRecord(const Ion::Storage::Record record);
  SequenceContext* sequenceContext() { return &m_sequenceContext; }
  void tidyDownstreamPoolFrom(
      Poincare::PoolObject* treePoolCursor = nullptr) override;
  void prepareForNewApp();
  void reset();

 private:
  // Expression getters
  const Poincare::UserExpression protectedExpressionForSymbolAbstract(
      const Poincare::SymbolAbstract& symbol, bool clone,
      Poincare::ContextWithParent* lastDescendantContext) override;
  const Poincare::UserExpression expressionForSymbolAndRecord(
      const Poincare::SymbolAbstract& symbol, Ion::Storage::Record r,
      Context* ctx);
  static const Poincare::UserExpression ExpressionForUserNamed(
      Ion::Storage::Record r);
  static const Poincare::UserExpression ExpressionForFunction(
      const Poincare::UserExpression& parameter, Ion::Storage::Record r);
  const Poincare::UserExpression expressionForSequence(
      const Poincare::SymbolAbstract& symbol, Ion::Storage::Record r,
      Context* ctx);
  // Expression setters
  /* This modifies the expression. */
  Ion::Storage::Record::ErrorStatus setExpressionForUserNamed(
      Poincare::UserExpression& expression,
      const Poincare::SymbolAbstract& symbol,
      Ion::Storage::Record previousRecord);
  Ion::Storage::Record::ErrorStatus setExpressionForFunction(
      const Poincare::UserExpression& expression,
      const Poincare::SymbolAbstract& symbol,
      Ion::Storage::Record previousRecord);
  // Record getter
  static Ion::Storage::Record SymbolAbstractRecordWithBaseName(
      const char* name);
  double approximateSequenceAtRank(const char* identifier,
                                   int rank) const override;
  SequenceContext m_sequenceContext;
};

}  // namespace Shared

#endif
