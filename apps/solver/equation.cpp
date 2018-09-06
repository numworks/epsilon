#include "equation.h"

using namespace Poincare;

namespace Solver {

Equation::Equation() :
  Shared::ExpressionModel(),
  m_standardForm()
{
}

void Equation::setContent(const char * c) {
  /* ExpressionModel::setContent takes care of tidying m_expression and m_layout. */
  tidyStandardForm();
  ExpressionModel::setContent(c);
}

void Equation::tidy() {
  ExpressionModel::tidy();
  tidyStandardForm();
}

Expression Equation::standardForm(Context * context) const {
  if (m_standardForm.isUninitialized()) {
    const Expression e = expression(context);
    if (e.type() == ExpressionNode::Type::Equal) {
      m_standardForm = static_cast<const Equal&>(e).standardEquation(*context, Preferences::sharedPreferences()->angleUnit());
    } else if (e.type() == ExpressionNode::Type::Rational && static_cast<const Rational&>(e).isOne()) {
      // The equality was reduced which means the equality was always true.
      m_standardForm = Rational(0);
    } else {
      // The equality has an undefined operand
      assert(e.type() == ExpressionNode::Type::Undefined);
    }
  }
  return m_standardForm;
}

void Equation::tidyStandardForm() {
  // Free the pool of the m_standardForm
  m_standardForm = Expression();
}

}
