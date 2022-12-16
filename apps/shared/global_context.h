#ifndef APPS_SHARED_GLOBAL_CONTEXT_H
#define APPS_SHARED_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/float.h>
#include <poincare/decimal.h>
#include <poincare/symbol.h>
#include <ion/storage/file_system.h>
#include <assert.h>
#include "sequence_store.h"
#include "sequence_context.h"

namespace Shared {

class ContinuousFunctionStore;
class GlobalContext final : public Poincare::Context {
public:
  constexpr static const char * k_extensions[] = {Ion::Storage::expExtension, Ion::Storage::matExtension, Ion::Storage::funcExtension, Ion::Storage::lisExtension, Ion::Storage::seqExtension};
  constexpr static int k_numberOfExtensions = sizeof(k_extensions) / sizeof(char *);

  // Storage information
  static bool SymbolAbstractNameIsFree(const char * baseName);

  static const Poincare::Layout LayoutForRecord(Ion::Storage::Record record);

  // Destroy records
  static void DestroyRecordsBaseNamedWithoutExtension(const char * baseName, const char * extension);

  GlobalContext() : m_sequenceContext(this, sequenceStore()) {};
  /* Expression for symbol
   * The expression recorded in global context is already an expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) override;
  bool setExpressionForSymbolAbstract(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol) override;
  static SequenceStore * sequenceStore();
  static ContinuousFunctionStore * continuousFunctionStore();
  void storageDidChangeForRecord(const Ion::Storage::Record record);
  SequenceContext * sequenceContext() { return &m_sequenceContext; }
  void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) override;
private:
  // Expression getters
  const Poincare::Expression protectedExpressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, Poincare::ContextWithParent * lastDescendantContext) override;
  const Poincare::Expression expressionForSymbolAndRecord(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r, Context * ctx);
  static const Poincare::Expression ExpressionForActualSymbol(Ion::Storage::Record r);
  static const Poincare::Expression ExpressionForFunction(const Poincare::Expression & parameter, Ion::Storage::Record r);
  const Poincare::Expression expressionForSequence(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r, Context * ctx);
  // Expression setters
  Ion::Storage::Record::ErrorStatus setExpressionForActualSymbol(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol, Ion::Storage::Record previousRecord);
  Ion::Storage::Record::ErrorStatus setExpressionForFunction(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol, Ion::Storage::Record previousRecord);
  // Record getter
  static Ion::Storage::Record SymbolAbstractRecordWithBaseName(const char * name);
  SequenceContext m_sequenceContext;

};

}

#endif
