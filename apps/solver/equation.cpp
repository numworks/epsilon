#include "equation.h"
#include <poincare/constant.h>
#include <poincare/equal.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <poincare/rational.h>
#include <ion/unicode/utf8_helper.h>

using namespace Poincare;
using namespace Shared;

namespace Solver {

constexpr char Equation::extension[];

Equation::Equation(Ion::Storage::Record record) :
  ExpressionModelHandle(record),
  m_standardForm()
{
}

void Equation::tidyExpressionModel() {
  ExpressionModelHandle::tidyExpressionModel();
  // Free the pool of the m_standardForm
  m_standardForm = Expression();
}

Expression Equation::standardForm(Context * context) const {
  if (m_standardForm.isUninitialized()) {
    const Expression e = expressionReduced(context);
    if (e.type() == ExpressionNode::Type::Unreal) {
      m_standardForm = Unreal::Builder();
      return m_standardForm;
    }
    if (e.recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) { return e.type() == ExpressionNode::Type::Undefined || e.type() == ExpressionNode::Type::Infinity || Expression::IsMatrix(e, context, replaceSymbols); }, *context, true)) {
      m_standardForm = Undefined::Builder();
      return m_standardForm;
    }
    if (e.type() == ExpressionNode::Type::Equal) {
      Preferences * preferences = Preferences::sharedPreferences();
      m_standardForm = static_cast<const Equal&>(e).standardEquation(*context, Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), expressionClone(), *context), preferences->angleUnit());
    } else {
      assert(e.type() == ExpressionNode::Type::Rational && static_cast<const Rational&>(e).isOne());
      // The equality was reduced which means the equality was always true.
      m_standardForm = Rational::Builder(0);
    }
  }
  return m_standardForm;
}

bool Equation::containsIComplex(Context * context) const {
  return expressionClone().recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) { return e.type() == ExpressionNode::Type::Constant && static_cast<const Constant &>(e).isIComplex(); }, *context, true);
}

}
