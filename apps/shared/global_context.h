#ifndef APPS_SHARED_GLOBAL_CONTEXT_H
#define APPS_SHARED_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/float.h>
#include <poincare/decimal.h>
#include <poincare/symbol.h>
#include <ion/storage.h>

namespace Shared {

class Integer;

class GlobalContext final : public Poincare::Context {
public:
  // TODO: should this be store elsewhere?
  static constexpr char funcExtension[] = "func";
  static constexpr char expExtension[] = "exp";
  /* The expression recorded in global context is already a expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Poincare::Expression expressionForSymbol(const Poincare::SymbolAbstract & symbol) override;
  void setExpressionForSymbol(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol, Poincare::Context & context) override;
  static Ion::Storage::Record RecordWithName(const char * name);
  static bool storageMemoryFull();
  //TODO static constexpr uint16_t k_maxNumberOfSequences = 10;
private:
  //static constexpr char seqExtension[] = "seq";
  static const char * ExtensionForExpression(const Poincare::Expression & exp);
  const Poincare::Expression expressionForSymbolAndRecord(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r);
  const Poincare::Expression expressionForActualSymbol(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r);
  const Poincare::Expression expressionForFunction(const Poincare::SymbolAbstract & symbol, Ion::Storage::Record r);
};

}

#endif
