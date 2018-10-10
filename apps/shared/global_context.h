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

class Integer;

class GlobalContext final : public Poincare::Context {
public:
  static constexpr char funcExtension[] = "func"; // TODO: store this elsewhere?
  static constexpr char expExtension[] = "exp"; // TODO: store this elsewhere?
  //static constexpr char seqExtension[] = "seq";

  // Storage information
  static bool RecordBaseNameIsFree(const char * baseName);
  static bool storageMemoryFull();

  // Expression from record
  static Poincare::Expression ExpressionFromRecord(Ion::Storage::Record record);
  static Poincare::Expression ExpressionFromSymbolRecord(Ion::Storage::Record record);
  static Poincare::Expression ExpressionFromFunctionRecord(Ion::Storage::Record record);

  // Set expression for record
  static Ion::Storage::Record::ErrorStatus SetExpressionForFunctionRecord(Poincare::Expression e, Ion::Storage::Record record);

  /* Expression for symbol
   * The expression recorded in global context is already an expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Poincare::Expression expressionForSymbol(const Poincare::SymbolAbstract & symbol) override;
  void setExpressionForSymbol(
      const Poincare::Expression & expression,
      const Poincare::SymbolAbstract & symbol,
      Poincare::Context & context) override;

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
  static Ion::Storage::Record RecordWithBaseName(const char * name);
};

}

#endif
