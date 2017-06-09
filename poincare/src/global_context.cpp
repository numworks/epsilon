#include <poincare/global_context.h>
#include <poincare/matrix.h>
#include <assert.h>
#include <math.h>
#include <ion.h>

namespace Poincare {

GlobalContext::GlobalContext() :
  m_pi(Complex::Float(M_PI)),
  m_e(Complex::Float(M_E))
{
  for (int i = 0; i < k_maxNumberOfScalarExpressions; i++) {
    m_expressions[i] = nullptr;
  }
  for (int i = 0; i < k_maxNumberOfMatrixExpressions ; i++) {
    m_matrixExpressions[i] = nullptr;
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
  }
}

Complex * GlobalContext::defaultExpression() {
  static Complex * defaultExpression = new Complex(Complex::Float(0.0f));
  return defaultExpression;
}

int GlobalContext::symbolIndex(const Symbol * symbol) const {
  int index = symbol->name() - 'A';
  return index;
}

const Expression * GlobalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Ion::Charset::SmallPi) {
    return &m_pi;
  }
  if (symbol->name() == Ion::Charset::Exponential) {
    return &m_e;
  }
  if (symbol->isMatrixSymbol()) {
    int indexMatrix = symbol->name() - (char)Symbol::SpecialSymbols::M0;
    return m_matrixExpressions[indexMatrix];
  }
  int index = symbolIndex(symbol);
  if (index < 0 || index >= k_maxNumberOfScalarExpressions) {
    return nullptr;
  }
  if (m_expressions[index] == nullptr) {
    return defaultExpression();
  }
  return m_expressions[index];
}

void GlobalContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->isMatrixSymbol()) {
    int indexMatrix = symbol->name() - (char)Symbol::SpecialSymbols::M0;
    assert(indexMatrix >= 0 && indexMatrix < k_maxNumberOfMatrixExpressions);
    if (m_matrixExpressions[indexMatrix] != nullptr) {
      delete m_matrixExpressions[indexMatrix];
      m_matrixExpressions[indexMatrix] = nullptr;
    }
    if (expression != nullptr && expression->type() == Expression::Type::Matrix) {
      m_matrixExpressions[indexMatrix] = expression->clone();
    }
    return;
  }
  int index = symbolIndex(symbol);
  if (index < 0 || index >= k_maxNumberOfScalarExpressions) {
    return;
  }
  if (m_expressions[index] != nullptr) {
    delete m_expressions[index];
    m_expressions[index] = nullptr;
  }
  if (expression == nullptr) {
    return;
  }
  if (expression->type() == Expression::Type::Complex) {
    m_expressions[index] = expression->clone();
  } else {
    m_expressions[index] = new Complex(Complex::Float(NAN));
  }
}

}
