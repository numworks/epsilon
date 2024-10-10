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
  return (e.isBasedInteger() ||
          (e.isOpposite() && isIntegerInput(e.cloneChildAtIndex(0))));
}

static bool isFractionInput(const Expression e) {
  if (e.isOpposite()) {
    return isFractionInput(e.cloneChildAtIndex(0));
  }
  if (!e.isDiv()) {
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

  bool negative = e.isOpposite();
  const UserExpression div = negative ? e.cloneChildAtIndex(0) : e;
  assert(div.isDiv());

  SystemExpression rational =
      AdditionalResultsHelper::CreateRational(div, negative);
  SystemExpression mixedFraction = AdditionalResultsHelper::CreateMixedFraction(
      rational,
      GlobalPreferences::SharedGlobalPreferences()->mixedFractions() ==
          Preferences::MixedFractions::Enabled);
  UserExpression euclideanDiv =
      AdditionalResultsHelper::CreateEuclideanDivision(rational);

  int index = 0;
  m_layouts[index++] = PoincareHelpers::CreateLayout(
      mixedFraction.cloneAndBeautify({}), App::app()->localContext());
  m_layouts[index++] =
      PoincareHelpers::CreateLayout(euclideanDiv, App::app()->localContext());
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
