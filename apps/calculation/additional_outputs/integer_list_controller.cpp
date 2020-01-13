#include "integer_list_controller.h"
#include <poincare/based_integer.h>
#include <poincare/integer.h>
#include <poincare/empty_layout.h>
#include <poincare/factor.h>
#include "../app.h"
#include "../../shared/poincare_helpers.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

int IntegerListController::numberOfRows() const {
  return 3 + factorExpressionIsComputable();
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

void IntegerListController::computeLayoutAtIndex(int index) {
  if (!m_layouts[index].isUninitialized()) {
    return;
  }
  assert(m_expression.type() == ExpressionNode::Type::BasedInteger);
  // For index = k_indexOfFactorExpression, the layout is assumed to be alreday memoized because it is needed to compute the numberOfRows
  assert(index < k_indexOfFactorExpression);
  Integer i = static_cast<BasedInteger &>(m_expression).integer();
  m_layouts[index] = i.createLayout(baseAtIndex(index));
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

bool IntegerListController::factorExpressionIsComputable() const {
  if (!m_layouts[k_indexOfFactorExpression].isUninitialized()) {
    // The factor expression is already memoized
    return !m_layouts[k_indexOfFactorExpression].isEmpty();
  }
  Poincare::Context * context = App::app()->localContext();
  Expression factor = Factor::Builder(m_expression.clone());
  PoincareHelpers::Simplify(&factor, context, ExpressionNode::ReductionTarget::User);
  if (!factor.isUndefined()) {
    m_layouts[k_indexOfFactorExpression] = PoincareHelpers::CreateLayout(factor);
    return true;
  }
  m_layouts[k_indexOfFactorExpression] = EmptyLayout::Builder();
  return false;

}
}
