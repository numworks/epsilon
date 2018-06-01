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
    Expression * e = expression(context);
    if (e->type() == Expression::Type::Equal) {
      m_standardForm = static_cast<const Equal *>(e)->standardEquation(*context);
    } else {
      // The equality was reduced which means the equality was always true.
      assert(e->type() == Expression::Type::Rational && static_cast<Rational *>(e)->isOne());
      m_standardForm = new Rational(0);
    }
  }
  return m_standardForm;
}

}
