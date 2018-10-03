#ifndef POINCARE_GLOBAL_CONTEXT_H
#define POINCARE_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/float.h>
#include <poincare/decimal.h>
#include <poincare/symbol.h>
#include <ion/storage.h>

namespace Poincare {

class Integer;

class GlobalContext final : public Context {
public:
  static constexpr char funcExtension[] = "func";
  /* The expression recorded in global context is already a expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Expression expressionForSymbol(const Symbol & symbol) override;
  void setExpressionForSymbol(const Expression & expression, const Symbol & symbol, Context & context) override;
  static bool storageMemoryFull();
  //TODO static constexpr uint16_t k_maxNumberOfSequences = 10;
private:
  static constexpr char expExtension[] = "exp";
  //static constexpr char seqExtension[] = "seq";
  static const char * ExtensionForExpression(const Expression & exp);
  static Ion::Storage::Record RecordWithName(const char * name);
};

}

#endif
