#include <poincare/global_context.h>
#include <poincare/matrix.h>
#include <poincare/undefined.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

GlobalContext::GlobalContext() :
  m_pi(M_PI),
  m_e(M_E),
  m_i(0.0, 1.0)
{
  for (int i = 0; i < k_maxNumberOfScalarExpressions; i++) {
    m_expressions[i] = nullptr;
  }
  for (int i = 0; i < k_maxNumberOfMatrixExpressions ; i++) {
    m_matrixExpressions[i] = nullptr;
    m_matrixLayout[i] = nullptr;
  }
}

GlobalContext::~GlobalContext() {
  for (int i = 0; i < k_maxNumberOfScalarExpressions; i++) {
    if (m_expressions[i] != nullptr) {
      delete m_expressions[i];
    }
    m_expressions[i] = nullptr;
  }
  for (int i = 0; i < k_maxNumberOfMatrixExpressions; i++) {
    if (m_matrixExpressions[i] != nullptr) {
      delete m_matrixExpressions[i];
    }
    m_matrixExpressions[i] = nullptr;
    if (m_matrixLayout[i] != nullptr) {
      delete m_matrixLayout[i];
    }
    m_matrixLayout[i] = nullptr;
  }
}

Decimal * GlobalContext::defaultExpression() {
  static Decimal defaultExpression(Integer(0), 0);
  return &defaultExpression;
}

int GlobalContext::symbolIndex(const Symbol * symbol) const {
  if (symbol->isMatrixSymbol()) {
    return symbol->name() - (char)Symbol::SpecialSymbols::M0;
  }
  if (symbol->isScalarSymbol()) {
    return symbol->name() - 'A';
  }
  return -1;
}

const Expression * GlobalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Ion::Charset::IComplex) {
    return &m_i;
  }
  if (symbol->name() == Ion::Charset::SmallPi) {
    return &m_pi;
  }
  if (symbol->name() == Ion::Charset::Exponential) {
    return &m_e;
  }
  int index = symbolIndex(symbol);
  if (symbol->isMatrixSymbol()) {
    return m_matrixExpressions[index];
  }
  if (index < 0 || index >= k_maxNumberOfScalarExpressions) {
    return nullptr;
  }
  if (m_expressions[index] == nullptr) {
    return defaultExpression();
  }
  return m_expressions[index];
}

ExpressionLayout * GlobalContext::expressionLayoutForSymbol(const Symbol * symbol, int numberOfSignificantDigits) {
  if (symbol->isMatrixSymbol()) {
    int index = symbolIndex(symbol);
    if (m_matrixLayout[index] == nullptr && m_matrixExpressions[index] != nullptr) {
      m_matrixLayout[index] = m_matrixExpressions[index]->createLayout(PrintFloat::Mode::Decimal, numberOfSignificantDigits);
    }
    return m_matrixLayout[index];
  }
  return nullptr;
}

void GlobalContext::setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) {
  int index = symbolIndex(symbol);
 if (symbol->isMatrixSymbol()) {
    int indexMatrix = symbol->name() - (char)Symbol::SpecialSymbols::M0;
    assert(indexMatrix >= 0 && indexMatrix < k_maxNumberOfMatrixExpressions);
    Expression * evaluation = expression ? expression->approximate<double>(context) : nullptr; // evaluate before deleting anything (to be able to evaluate M1+2->M1)
    if (m_matrixExpressions[indexMatrix] != nullptr) {
      delete m_matrixExpressions[indexMatrix];
      m_matrixExpressions[indexMatrix] = nullptr;
    }
    if (m_matrixLayout[indexMatrix] != nullptr) {
      delete m_matrixLayout[indexMatrix];
      m_matrixLayout[indexMatrix] = nullptr;
    }
    if (evaluation != nullptr) {
      if (evaluation->type() != Expression::Type::Matrix) {
        m_matrixExpressions[indexMatrix] = new Matrix(&evaluation, 1, 1, false);
      } else {
        m_matrixExpressions[indexMatrix] = static_cast<Matrix *>(evaluation);
      }
    }
    return;
  }
  if (index < 0 || index >= k_maxNumberOfScalarExpressions) {
    return;
  }
  Expression * evaluation = expression ? expression->approximate<double>(context) : nullptr; // evaluate before deleting anything (to be able to evaluate A+2->A)
  if (m_expressions[index] != nullptr) {
    delete m_expressions[index];
    m_expressions[index] = nullptr;
  }
  if (evaluation == nullptr) {
    return;
  }
  if (evaluation->type() == Expression::Type::Matrix) {
    m_expressions[index] = new Undefined();
    delete evaluation;
  } else {
    m_expressions[index] = evaluation;
  }
}

}
