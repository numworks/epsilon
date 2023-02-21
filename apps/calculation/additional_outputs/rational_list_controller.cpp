#include "rational_list_controller.h"

#include <poincare/based_integer.h>
#include <poincare/integer.h>
#include <string.h>

#include "../../shared/poincare_helpers.h"
#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

Integer extractInteger(const Expression e) {
  if (e.type() == ExpressionNode::Type::Opposite) {
    Integer i = extractInteger(e.childAtIndex(0));
    i.setNegative(!i.isNegative());
    return i;
  }
  assert(e.type() == ExpressionNode::Type::BasedInteger);
  return static_cast<const BasedInteger &>(e).integer();
}

void RationalListController::setExpression(Poincare::Expression e) {
  ExpressionsListController::setExpression(e);
  assert(!m_expression.isUninitialized());
  static_assert(k_maxNumberOfRows >= 2,
                "k_maxNumberOfRows must be greater than 2");

  bool negative = false;
  Expression div = m_expression;
  if (m_expression.type() == ExpressionNode::Type::Opposite) {
    negative = true;
    div = m_expression.childAtIndex(0);
  }

  assert(div.type() == ExpressionNode::Type::Division);
  Integer numerator = extractInteger(div.childAtIndex(0));
  numerator.setNegative(negative != numerator.isNegative());
  Integer denominator = extractInteger(div.childAtIndex(1));

  int index = 0;
  m_layouts[index++] = PoincareHelpers::CreateLayout(
      Integer::CreateMixedFraction(numerator, denominator),
      App::app()->localContext());
  m_layouts[index++] = PoincareHelpers::CreateLayout(
      Integer::CreateEuclideanDivision(numerator, denominator),
      App::app()->localContext());
}

I18n::Message RationalListController::messageAtIndex(int index) {
  switch (index) {
    case 0:
      return I18n::Message::MixedFraction;
    default:
      return I18n::Message::EuclideanDivision;
  }
}

int RationalListController::textAtIndex(char *buffer, size_t bufferSize,
                                        Escher::HighlightCell *cell,
                                        int index) {
  int length =
      ExpressionsListController::textAtIndex(buffer, bufferSize, cell, index);
  if (index == 1) {
    // Get rid of the left part of the equality
    char *equalPosition = strchr(buffer, '=');
    assert(equalPosition != nullptr);
    strlcpy(buffer, equalPosition + 1, bufferSize);
    return buffer + length - 1 - equalPosition;
  }
  return length;
}

}  // namespace Calculation
