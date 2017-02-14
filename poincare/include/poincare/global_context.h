#ifndef POINCARE_GLOBAL_CONTEXT_H
#define POINCARE_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/float.h>

class Integer;

/* The global context only stores symbols A-Z, L1-L9 and M1-M9 */

class GlobalContext : public Context {
public:
  GlobalContext();
  /* The expression recorded in global context is already a final expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Expression * expressionForSymbol(const Symbol * symbol) override;
  void setExpressionForSymbolName(Expression * expression, const Symbol * symbol) override;
  static constexpr uint16_t k_maxNumberOfScalarExpressions = 26;
  static constexpr uint16_t k_maxNumberOfListExpressions = 10;
  static constexpr uint16_t k_maxNumberOfMatrixExpressions = 10;
  static Float * defaultExpression();
private:
  int symbolIndex(const Symbol * symbol) const;
  Expression * m_expressions[k_maxNumberOfScalarExpressions];
  Float m_pi;
  Float m_e;
};

#endif
