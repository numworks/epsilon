#include "rational_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare_nodes.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

int RationalListController::numberOfRows() const {
  return 2;
}

Integer extractInteger(const Expression e) {
  assert(Expression::IsSignedBasedInteger(e));
  // e is of form [±12]/[±23]
  Integer i;
  if (e.type() == ExpressionNode::Type::BasedInteger) {
    i = static_cast<const BasedInteger &>(e).integer();
  } else {
    Expression f = e.childAtIndex(0);
    assert(f.type() == ExpressionNode::Type::BasedInteger);
    i = static_cast<const BasedInteger &>(f).integer();
    i.setNegative(true);
  }
  return i;
}

Layout RationalListController::layoutAtIndex(int index) {
  // TODO: implement mixed fraction
  assert(m_expression.type() == ExpressionNode::Type::Division);
  Integer numerator = extractInteger(m_expression.childAtIndex(0));
  Integer denominator = extractInteger(m_expression.childAtIndex(1));
  Expression e = Integer::CreateEuclideanDivision(numerator, denominator);
  return PoincareHelpers::CreateLayout(e);
}

I18n::Message RationalListController::messageAtIndex(int index) {
  switch (index) {
    case 0:
      return I18n::Message::MixedFraction;
    default:
      return I18n::Message::EuclideanDivision;
  }
}

}
