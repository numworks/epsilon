#include "rational_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/additional_results_helper.h>
#include <poincare/k_tree.h>
#include <string.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

static bool isIntegerInput(const Expression e) {
  return (e.type() == ExpressionNode::Type::BasedInteger ||
          (e.type() == ExpressionNode::Type::Opposite &&
           isIntegerInput(e.cloneChildAtIndex(0))));
}

static bool isFractionInput(const Expression e) {
  if (e.type() == ExpressionNode::Type::Opposite) {
    return isFractionInput(e.cloneChildAtIndex(0));
  }
  if (e.type() != ExpressionNode::Type::Division) {
    return false;
  }
  Expression num = e.cloneChildAtIndex(0);
  Expression den = e.cloneChildAtIndex(1);
  return isIntegerInput(num) && isIntegerInput(den);
}

void RationalListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasRational(exactOutput));
  UserExpression e = isFractionInput(input) ? input : exactOutput;
  assert(!e.isUninitialized());
  static_assert(k_maxNumberOfRows >= 2,
                "k_maxNumberOfRows must be greater than 2");

  bool negative = e.type() == ExpressionNode::Type::Opposite;
  const UserExpression div = negative ? e.cloneChildAtIndex(0) : e;
  assert(div.type() == ExpressionNode::Type::Division);

  SystemExpression rational =
      AdditionalResultsHelper::CreateRational(div, negative);
  SystemExpression mixedFraction = AdditionalResultsHelper::CreateMixedFraction(
      rational,
      GlobalPreferences::SharedGlobalPreferences()->mixedFractions() ==
          Preferences::MixedFractions::Enabled);
  SystemExpression euclideanDiv =
      AdditionalResultsHelper::CreateEuclideanDivision(rational);

  int index = 0;
  m_layouts[index++] = PoincareHelpers::CreateLayout(
      mixedFraction.cloneAndBeautify({}), App::app()->localContext());
  m_layouts[index++] = PoincareHelpers::CreateLayout(
      euclideanDiv.cloneAndBeautify({}), App::app()->localContext());
}

I18n::Message RationalListController::messageAtIndex(int index) {
  switch (index) {
    case 0:
      return I18n::Message::MixedFraction;
    default:
      return I18n::Message::EuclideanDivision;
  }
}

Layout RationalListController::layoutAtIndex(Escher::HighlightCell* cell,
                                             int index) {
  return ExpressionsListController::layoutAtIndex(cell, index);
#if 0  // TODO_PCJ
  if (index == 1) {
    // Get rid of the left part of the equality
    char *equalPosition = strchr(buffer, '=');
    assert(equalPosition != nullptr);
    strlcpy(buffer, equalPosition + 1, bufferSize);
    return buffer + length - 1 - equalPosition;
  }
  return length;
#endif
}

}  // namespace Calculation
