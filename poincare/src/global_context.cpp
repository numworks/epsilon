#include <poincare/global_context.h>
#include <poincare/undefined.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

GlobalContext::GlobalContext() :
 m_matrixLayouts{},
  m_pi(M_PI),
  m_e(M_E)
{
}

Decimal GlobalContext::defaultExpression() {
  static Decimal defaultExpression(Integer(0), 0);
  return defaultExpression;
}

int GlobalContext::symbolIndex(const Symbol symbol) const {
  if (Symbol::isMatrixSymbol(symbol.name())) {
    return symbol.name() - (char)Symbol::SpecialSymbols::M0;
  }
  if (Symbol::isScalarSymbol(symbol.name())) {
    return symbol.name() - 'A';
  }
  return -1;
}

const Expression GlobalContext::expressionForSymbol(const Symbol symbol) {
  if (symbol.name() == Ion::Charset::SmallPi) {
    return m_pi;
  }
  if (symbol.name() == Ion::Charset::Exponential) {
    return m_e;
  }
  int index = symbolIndex(symbol);
  if (Symbol::isMatrixSymbol(symbol.name())) {
    return m_matrixExpressions[index];
  }
  if (index < 0 || index >= k_maxNumberOfScalarExpressions) {
    return Expression();
  }
  if (!m_expressions[index].isUninitialized()) {
    return m_expressions[index];
  }
  return defaultExpression();
}

LayoutRef GlobalContext::layoutForSymbol(const Symbol symbol, int numberOfSignificantDigits) {
  if (Symbol::isMatrixSymbol(symbol.name())) {
    int index = symbolIndex(symbol);
    if (m_matrixLayouts[index].isUninitialized()) {
      m_matrixLayouts[index] = m_matrixExpressions[index].createLayout(Preferences::PrintFloatMode::Decimal, numberOfSignificantDigits);
    }
    return m_matrixLayouts[index];
  }
  return LayoutRef();
}

void GlobalContext::setExpressionForSymbolName(const Expression expression, const Symbol symbol, Context & context) {
  int index = symbolIndex(symbol);
 if (Symbol::isMatrixSymbol(symbol.name())) {
    int indexMatrix = symbol.name() - (char)Symbol::SpecialSymbols::M0;
    assert(indexMatrix >= 0 && indexMatrix < k_maxNumberOfMatrixExpressions);
    Expression evaluation = expression.isUninitialized() ? Expression() : expression.approximate<double>(context, Preferences::sharedPreferences()->angleUnit(), Preferences::sharedPreferences()->complexFormat()); // evaluate before deleting anything (to be able to evaluate M1+2->M1)
    if (!evaluation.isUninitialized()) {
      if (evaluation.type() != ExpressionNode::Type::Matrix) {
        m_matrixExpressions[indexMatrix] = Matrix(evaluation);
      } else {
        m_matrixExpressions[indexMatrix] = Matrix(static_cast<MatrixNode *>(evaluation.node()));
      }
    }
    return;
  }
  if (index < 0 || index >= k_maxNumberOfScalarExpressions) {
    return;
  }
  Expression evaluation = expression.isUninitialized() ? Expression() : expression.approximate<double>(context, Preferences::sharedPreferences()->angleUnit(), Preferences::sharedPreferences()->complexFormat()); // evaluate before deleting anything (to be able to evaluate A+2->A)
  if (evaluation.isUninitialized()) {
    return;
  }
  if (evaluation.type() == ExpressionNode::Type::Matrix) {
    m_expressions[index] = Undefined();
  } else {
    m_expressions[index] = evaluation;
  }
}

}
