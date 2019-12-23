#include "integer_list_controller.h"
#include <poincare/based_integer.h>
#include <poincare/integer.h>
#include <poincare/factor.h>
#include "../app.h"
#include "../../shared/poincare_helpers.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

int IntegerListController::numberOfRows() const {
  return 4;
}

Integer::Base baseAtIndex(int index) {
  switch (index) {
    case 0:
      return Integer::Base::Decimal;
    case 1:
      return Integer::Base::Hexadecimal;
    default:
      assert(index == 2);
      return Integer::Base::Binary;
  }
}

Layout IntegerListController::layoutAtIndex(int index) {
  assert(m_expression.type() == ExpressionNode::Type::BasedInteger);
  Poincare::Context * context = App::app()->localContext();
  if (index == 3) {
    Expression factor = Factor::Builder(m_expression.clone());
    PoincareHelpers::Simplify(&factor, context, ExpressionNode::ReductionTarget::User);
    return PoincareHelpers::CreateLayout(factor);
  }
  Integer i = static_cast<BasedInteger &>(m_expression).integer();
  return i.createLayout(baseAtIndex(index));
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

}
