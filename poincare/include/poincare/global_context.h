#ifndef POINCARE_GLOBAL_CONTEXT_H
#define POINCARE_GLOBAL_CONTEXT_H

#include <poincare/context.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <cmath>

namespace Poincare {

class Integer;

/* The global context only stores symbols A-Z, L1-L9 and M1-M9 */

class GlobalContext final : public Context {
public:
  GlobalContext() :
    m_pi(Complex<double>::Float(M_PI)),
    m_e(Complex<double>::Float(M_E)),
    m_i(Complex<double>::Cartesian(0.0, 1.0)) {
    for (int i = 0; i < k_maxNumberOfScalarExpressions; i++) {
      m_expressions[i] = nullptr;
    }
    for (int i = 0; i < k_maxNumberOfMatrixExpressions ; i++) {
      m_matrixExpressions[i] = nullptr;
      m_matrixLayout[i] = nullptr;
    }
  }
  ~GlobalContext() {
    for (int i = 0; i < k_maxNumberOfScalarExpressions; i++) {
      delete m_expressions[i];
      m_expressions[i] = nullptr;
    }
    for (int i = 0; i < k_maxNumberOfMatrixExpressions; i++) {
      delete m_matrixExpressions[i];
      m_matrixExpressions[i] = nullptr;
      delete m_matrixLayout[i];
      m_matrixLayout[i] = nullptr;
    }
  }
  GlobalContext(const GlobalContext& other) = delete;
  GlobalContext(GlobalContext&& other) = delete;
  GlobalContext& operator=(const GlobalContext& other) = delete;
  GlobalContext& operator=(GlobalContext&& other) = delete;
  /* The expression recorded in global context is already a expression.
   * Otherwise, we would need the context and the angle unit to evaluate it */
  const Expression * expressionForSymbol(const Symbol * symbol) override;
  ExpressionLayout * expressionLayoutForSymbol(const Symbol * symbol);
  void setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) override;
  static constexpr uint16_t k_maxNumberOfScalarExpressions = 26;
  static constexpr uint16_t k_maxNumberOfListExpressions = 10;
  static constexpr uint16_t k_maxNumberOfMatrixExpressions = 10;
private:
  static Complex<double> * defaultExpression();
  int symbolIndex(const Symbol * symbol) const;
  Complex<double> * m_expressions[k_maxNumberOfScalarExpressions];
  Matrix * m_matrixExpressions[k_maxNumberOfMatrixExpressions];
  /* Matrix layout memoization */
  ExpressionLayout * m_matrixLayout[k_maxNumberOfMatrixExpressions];
  Complex<double> m_pi;
  Complex<double> m_e;
  Complex<double> m_i;
};

}

#endif
