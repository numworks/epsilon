#include "integer_list_controller.h"

#include <poincare/based_integer.h>
#include <poincare/factor.h>
#include <poincare/integer.h>

#include "../../shared/poincare_helpers.h"
#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

OMG::Base baseAtIndex(int index) {
  switch (index) {
    case 0:
      return OMG::Base::Decimal;
    case 1:
      return OMG::Base::Hexadecimal;
    default:
      assert(index == 2);
      return OMG::Base::Binary;
  }
}

void IntegerListController::setExpression(Poincare::Expression e) {
  ExpressionsListController::setExpression(e);
  static_assert(
      k_maxNumberOfRows >= k_indexOfFactorExpression + 1,
      "k_maxNumberOfRows must be greater than k_indexOfFactorExpression");
  assert(!m_expression.isUninitialized() &&
         m_expression.type() == ExpressionNode::Type::BasedInteger);
  Integer integer = static_cast<BasedInteger &>(m_expression).integer();
  for (int index = 0; index < k_indexOfFactorExpression; ++index) {
    m_layouts[index] = integer.createLayout(baseAtIndex(index));
  }
  // Computing factorExpression
  Expression factor = Factor::Builder(m_expression);
  PoincareHelpers::CloneAndSimplify(&factor, App::app()->localContext(),
                                    ReductionTarget::User);
  if (!factor.isUndefined()) {
    m_layouts[k_indexOfFactorExpression] =
        PoincareHelpers::CreateLayout(factor, App::app()->localContext());
  }
}

I18n::Message IntegerListController::messageAtIndex(int index) {
  switch (index) {
    case 0:
      return I18n::Message::DecimalBase;
    case 1:
      return I18n::Message::HexadecimalBase;
    case 2:
      return I18n::Message::BinaryBase;
    default:
      return I18n::Message::PrimeFactors;
  }
}

}  // namespace Calculation
