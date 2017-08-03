#include <poincare/global_context.h>
#include <poincare/matrix.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

GlobalContext::GlobalContext() :
  m_pi(Complex<double>::Float(M_PI)),
  m_e(Complex<double>::Float(M_E))
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

Complex<double> * GlobalContext::defaultExpression() {
  static Complex<double> * defaultExpression = new Complex<double>(Complex<double>::Float(0.0));
  return defaultExpression;
}

int GlobalContext::symbolIndex(const Symbol * symbol) const {
  int index = symbol->name() - 'A';
  return index;
}

const Evaluation<double> * GlobalContext::evaluationForSymbol(const Symbol * symbol) {
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
    if (expression != nullptr) {
      Evaluation<double> * evaluation = expression->evaluate<double>(*this);
      if (evaluation->numberOfOperands() == 1) {
        m_matrixExpressions[indexMatrix] = new ComplexMatrix<double>(evaluation->complexOperand(0), 1, 1);
        delete evaluation;
      } else {
        m_matrixExpressions[indexMatrix] = (ComplexMatrix<double> *)evaluation;
      }
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
  Evaluation<double> * evaluation = expression->evaluate<double>(*this);
  if (evaluation->numberOfOperands() == 1) {
    m_expressions[index] = new Complex<double>(*(evaluation->complexOperand(0)));
  } else {
    m_expressions[index] = new Complex<double>(Complex<double>::Float(NAN));
  }
  delete evaluation;
}

}
