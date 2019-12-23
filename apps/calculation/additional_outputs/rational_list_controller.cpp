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

Layout RationalListController::layoutAtIndex(int index) {
  // TODO: do something simpler using rational?
  // TODO: implement mixed fraction
  assert(m_expression.type() == ExpressionNode::Type::Division);
  Expression c0 = m_expression.childAtIndex(0);
  Expression c1 = m_expression.childAtIndex(1);
  assert(c0.type() == ExpressionNode::Type::BasedInteger);
  assert(c1.type() == ExpressionNode::Type::BasedInteger);
  Integer num = static_cast<BasedInteger &>(c0).integer();
  Integer denom = static_cast<BasedInteger &>(c1).integer();
  Poincare::Context * context = App::app()->localContext();
  DivisionQuotient quo = DivisionQuotient::Builder(Rational::Builder(num), Rational::Builder(denom));
  PoincareHelpers::Simplify(&quo, context, ExpressionNode::ReductionTarget::User);
  DivisionRemainder rem = DivisionRemainder::Builder(Rational::Builder(num), Rational::Builder(denom));
  PoincareHelpers::Simplify(&rem, context, ExpressionNode::ReductionTarget::User);
  Expression e = Equal::Builder(Rational::Builder(num), Addition::Builder(Multiplication::Builder(Rational::Builder(denom), quo), rem));
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
