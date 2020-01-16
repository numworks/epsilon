#include "rational_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare_nodes.h>
#include <string.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

int RationalListController::numberOfRows() const {
  return 2;
}

Integer extractInteger(const Expression e) {
  assert(e.type() == ExpressionNode::Type::BasedInteger);
  return static_cast<const BasedInteger &>(e).integer();
}

void RationalListController::computeLayoutAtIndex(int index) {
  bool negative = false;
  Expression div = m_expression;
  if (m_expression.type() == ExpressionNode::Type::Opposite) {
    negative = true;
    div = m_expression.childAtIndex(0);
  }
  assert(div.type() == ExpressionNode::Type::Division);
  Integer numerator = extractInteger(div.childAtIndex(0));
  numerator.setNegative(negative);
  Integer denominator = extractInteger(div.childAtIndex(1));
  Expression e;
  if (index == 0) {
    e = Integer::CreateMixedFraction(numerator, denominator);
  } else {
    assert(index == 1);
    e = Integer::CreateEuclideanDivision(numerator, denominator);
  }
  m_layouts[index] = PoincareHelpers::CreateLayout(e);
}

I18n::Message RationalListController::messageAtIndex(int index) {
  switch (index) {
    case 0:
      return I18n::Message::MixedFraction;
    default:
      return I18n::Message::EuclideanDivision;
  }
}

int RationalListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  int length = ExpressionsListController::textAtIndex(buffer, bufferSize, index);
  if (index == 1) {
    // Get rid of the left part of the equality
    char * equalPosition = strchr(buffer, '=');
    assert(equalPosition != nullptr);
    strlcpy(buffer, equalPosition + 1, bufferSize);
    return buffer + length - 1 - equalPosition;
  }
  return length;
}

}
