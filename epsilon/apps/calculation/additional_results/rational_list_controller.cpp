#include "rational_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/additional_results_helper.h>
#include <poincare/helpers/layout.h>
#include <poincare/k_tree.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_helpers.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void RationalListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasRational(exactOutput));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");
  UserExpression e = exactOutput;
  assert(!e.isUninitialized());
  bool reductionFailure = false;
  SystemExpression eReduced =
      e.cloneAndReduce({.m_advanceReduce = false}, &reductionFailure);
  assert(!reductionFailure);
  assert(eReduced.tree()->isRational());

  Layout approximateFraction =
      AdditionalResultsHelper::CreateRationalApproximation(eReduced);
  UserExpression mixedFraction = AdditionalResultsHelper::CreateMixedFraction(
      eReduced,
      GlobalPreferences::SharedGlobalPreferences()->mixedFractions() ==
          Preferences::MixedFractions::Enabled);
  UserExpression euclideanDiv =
      AdditionalResultsHelper::CreateEuclideanDivision(eReduced);

  int index = 0;
  if (!approximateFraction.isUninitialized()) {
    m_message[index] = I18n::Message::RationalApproximation;
    m_layouts[index++] = approximateFraction;
  }
  m_message[index] = I18n::Message::MixedFraction;
  m_layouts[index++] =
      PoincareHelpers::CreateLayout(mixedFraction, App::app()->localContext());
  m_message[index] = I18n::Message::EuclideanDivision;
  m_layouts[index++] =
      PoincareHelpers::CreateLayout(euclideanDiv, App::app()->localContext());
}

Layout RationalListController::layoutAtIndex(Escher::HighlightCell* cell,
                                             int index) {
  assert(index < k_maxNumberOfOutputRows && index >= 0);
  Layout l = ExpressionsListController::layoutAtIndex(cell, index);
  if (index == indexOfRationalApproximationCell()) {
    // Strip the ~ sign from the rational approximation
    Internal::Tree* result = l.tree()->cloneTree();
    assert(result->isRackLayout() && result->numberOfChildren() >= 2);
    assert(result->child(0)->isCodePointLayout() &&
           Internal::CodePointLayout::IsCodePoint(result->child(0), '~'));
    Internal::NAry::RemoveChildAtIndex(result, 0);
    return Layout::Builder(result);
  }
  if (index == indexOfEuclidianDivisionCell()) {
    // Strip the left part of the equality in euclidean division
    Internal::Tree* result = l.tree()->cloneTree();
    Internal::TreeRef end = pushEndMarker(result);
    Internal::Tree* child = result->nextNode();
    while (child != end) {
      bool isEqualitySign = child->isCodePointLayout() &&
                            Internal::CodePointLayout::IsCodePoint(child, '=');
      Internal::NAry::RemoveChildAtIndex(result, 0);
      if (isEqualitySign) {
        break;
      }
    }
    removeMarker(end);
    return Layout::Builder(result);
  }
  return l;
}

}  // namespace Calculation
