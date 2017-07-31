#ifndef POINCARE_GLOBAL_CONTEXT_H
#define POINCARE_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/complex.h>
#include <poincare/complex_matrix.h>

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
  /* The expression recorded in global context is already a final expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Evaluation * expressionForSymbol(const Symbol * symbol) override;
  void setExpressionForSymbolName(Evaluation * expression, const Symbol * symbol) override;
  static constexpr uint16_t k_maxNumberOfScalarExpressions = 26;
  static constexpr uint16_t k_maxNumberOfListExpressions = 10;
  static constexpr uint16_t k_maxNumberOfMatrixExpressions = 10;
  static Evaluation * defaultExpression();
private:
  int symbolIndex(const Symbol * symbol) const;
  Evaluation * m_expressions[k_maxNumberOfScalarExpressions];
  Evaluation * m_matrixExpressions[k_maxNumberOfMatrixExpressions];
  Complex m_pi;
  Complex m_e;
};

}

#endif
