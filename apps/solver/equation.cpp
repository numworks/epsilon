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
  tidyStandardForm();
}

void Equation::setContent(const char * c) {
  /* ExpressionModel::setContent takes care of tidying m_expression and m_layout. */
  tidyStandardForm();
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
    } else if (e->type() == Expression::Type::Rational && static_cast<Rational *>(e)->isOne()) {
      // The equality was reduced which means the equality was always true.
      m_standardForm = new Rational(0);
    } else {
      // The equality has an undefined operand
      assert(e->type() == Expression::Type::Undefined);
    }
  }
  return m_standardForm;
}

void Equation::tidyStandardForm() {
  if (m_standardForm) {
    delete m_standardForm;
    m_standardForm = nullptr;
  }
}

}
