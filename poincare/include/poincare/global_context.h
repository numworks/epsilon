#ifndef POINCARE_GLOBAL_CONTEXT_H
#define POINCARE_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/float.h>
#include <poincare/decimal.h>
#include <poincare/symbol.h>

namespace Poincare {

class Integer;

/* The global context only stores symbols A-Z, L0-L9 and M0-M9 */

class GlobalContext final : public Context {
public:
  /* The expression recorded in global context is already a expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Expression expressionForSymbol(const Symbol & symbol) override;
  void setExpressionForSymbolName(const Expression & expression, const Symbol & symbol, Context & context) override;
  static constexpr uint16_t k_maxNumberOfScalarExpressions = 26;
  //static constexpr uint16_t k_maxNumberOfListExpressions = 10;
  static constexpr uint16_t k_maxNumberOfMatrixExpressions = 10;
private:
  static constexpr int k_extensionSize = 4+1;
  struct FileName {
    char nameWithExtension[k_extensionSize+1];
  };
  FileName fileNameForSymbol(const Symbol & s) const;
};

}

#endif
