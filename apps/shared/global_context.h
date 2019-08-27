#ifndef APPS_SHARED_GLOBAL_CONTEXT_H
#define APPS_SHARED_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/float.h>
#include <poincare/decimal.h>
#include <poincare/symbol.h>
#include <ion/storage.h>
#include <assert.h>

namespace Shared {

class GlobalContext final : public Poincare::Context {
public:
  static constexpr int k_numberOfExtensions = 2;
  static constexpr const char * k_extensions[] = {Ion::Storage::expExtension, Ion::Storage::funcExtension};

  // Storage information
  static bool SymbolAbstractNameIsFree(const char * baseName);

  // Expression from record
  static Poincare::Expression ExpressionFromRecord(Ion::Storage::Record record);
  static Poincare::Expression ExpressionFromSymbolRecord(Ion::Storage::Record record);
  static Poincare::Expression ExpressionFromFunctionRecord(Ion::Storage::Record record);

  // Set expression for record
  static Ion::Storage::Record::ErrorStatus SetExpressionForFunctionRecord(Poincare::Expression e, Ion::Storage::Record record, const char * baseName);

  // Destroy records
  static void DestroyRecordsBaseNamedWithoutExtension(const char * baseName, const char * extension);

  /* Expression for symbol
   * The expression recorded in global context is already an expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone) override;
  void setExpressionForSymbolAbstract(
      const Poincare::Expression & expression,
      const Poincare::SymbolAbstract & symbol,
      Poincare::Context * context) override;

private:
  // Expression getters
  static const Poincare::Expression ExpressionForSymbolAndRecord(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r);
  static const Poincare::Expression ExpressionForActualSymbol(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r);
  static const Poincare::Expression ExpressionForFunction(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r) {
    assert(symbol.type() == Poincare::ExpressionNode::Type::Function);
    return ExpressionFromFunctionRecord(r);
  }
  // Expression setters
  static Ion::Storage::Record::ErrorStatus SetExpressionForActualSymbol(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol, Ion::Storage::Record previousRecord);
  static Ion::Storage::Record::ErrorStatus SetExpressionForFunction(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol, Ion::Storage::Record previousRecord);
  // Record getter
  static Ion::Storage::Record SymbolAbstractRecordWithBaseName(const char * name);
};

}

#endif
