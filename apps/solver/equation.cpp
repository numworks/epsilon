#include "equation.h"

#include <poincare/equal.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>

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
    if (e.recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) { return e.type() == ExpressionNode::Type::Undefined || e.type() == ExpressionNode::Type::Infinity || Expression::IsMatrix(e, context, replaceSymbols); }, *context, true)) {
      m_standardForm = Undefined();
      return m_standardForm;
    }
    if (e.type() == ExpressionNode::Type::Equal) {
      m_standardForm = static_cast<const Equal&>(e).standardEquation(*context, Preferences::sharedPreferences()->angleUnit());
    } else {
      assert(e.type() == ExpressionNode::Type::Rational && static_cast<const Rational&>(e).isOne());
      // The equality was reduced which means the equality was always true.
      m_standardForm = Rational(0);
    }
  }
  return m_standardForm;
}

void Equation::tidyStandardForm() {
  // Free the pool of the m_standardForm
  m_standardForm = Expression();
}

}
