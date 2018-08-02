#ifndef POINCARE_GLOBAL_CONTEXT_H
#define POINCARE_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/approximation.h>
#include <poincare/decimal.h>

namespace Poincare {

class Integer;

/* The global context only stores symbols A-Z, L1-L9 and M1-M9 */

class GlobalContext : public Context {
public:
  GlobalContext();
  /* The expression recorded in global context is already a expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const ExpressionReference expressionForSymbol(const SymbolReference symbol) override;
  LayoutRef layoutForSymbol(const SymbolReference symbol, int numberOfSignificantDigits);
  void setExpressionForSymbolName(const ExpressionReference expression, const SymbolReference symbol, Context & context) override;
  static constexpr uint16_t k_maxNumberOfScalarExpressions = 26;
  static constexpr uint16_t k_maxNumberOfListExpressions = 10;
  static constexpr uint16_t k_maxNumberOfMatrixExpressions = 10;
private:
  static Decimal * defaultExpression();
  int symbolIndex(const Symbol * symbol) const;
  ExpressionReference m_expressions[k_maxNumberOfScalarExpressions];
  MatrixReference m_matrixExpressions[k_maxNumberOfMatrixExpressions];
  /* Matrix layout memoization */
  LayoutRef m_matrixLayouts[k_maxNumberOfMatrixExpressions];
  Approximation<double> m_pi;
  Approximation<double> m_e;
  Approximation<double> m_i;
};

}

#endif
