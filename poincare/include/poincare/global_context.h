#ifndef POINCARE_GLOBAL_CONTEXT_H
#define POINCARE_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>

namespace Poincare {

class Integer;

/* The global context only stores symbols A-Z, L1-L9 and M1-M9 */

class GlobalContext : public Context {
public:
  GlobalContext();
  ~GlobalContext();
  GlobalContext(const GlobalContext& other) = delete;
  GlobalContext(GlobalContext&& other) = delete;
  GlobalContext& operator=(const GlobalContext& other) = delete;
  GlobalContext& operator=(GlobalContext&& other) = delete;
  /* The expression recorded in global context is already a expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Expression * expressionForSymbol(const Symbol * symbol) override;
  void setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) override;
  static constexpr uint16_t k_maxNumberOfScalarExpressions = 26;
  static constexpr uint16_t k_maxNumberOfListExpressions = 10;
  static constexpr uint16_t k_maxNumberOfMatrixExpressions = 10;
private:
  static Complex<double> * defaultExpression();
  int symbolIndex(const Symbol * symbol) const;
  Complex<double> * m_expressions[k_maxNumberOfScalarExpressions];
  Matrix * m_matrixExpressions[k_maxNumberOfMatrixExpressions];
  Complex<double> m_pi;
  Complex<double> m_e;
  Complex<double> m_i;
};

}

#endif
