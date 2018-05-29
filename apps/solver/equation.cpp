#include "equation.h"

using namespace Poincare;

namespace Solver {

Equation::Equation() :
  Shared::ExpressionModel(),
  m_standardForm(nullptr)
{
}

Equation& Equation::operator=(const Equation& other) {
  Shared::ExpressionModel::operator=(other);
  return *this;
}

Equation::~Equation() {
  if (m_standardForm) {
    delete m_standardForm;
    m_standardForm = nullptr;
  }
}

void Equation::setContent(const char * c) {
  tidy();
  ExpressionModel::setContent(c);
}

void Equation::tidy() {
  ExpressionModel::tidy();
  if (m_standardForm) {
    delete m_standardForm;
    m_standardForm = nullptr;
  }
}

Expression * Equation::standardForm(Context * context) const {
  if (m_standardForm == nullptr) {
    m_standardForm = static_cast<const Equal *>(expression(context))->standardEquation(*context);
  }
  return m_standardForm;
}

}
