#include "integer_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/k_tree.h>

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

void IntegerListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  static_assert(
      k_maxNumberOfRows >= k_indexOfFactorExpression + 1,
      "k_maxNumberOfRows must be greater than k_indexOfFactorExpression");
  assert(AdditionalResultsType::HasInteger(exactOutput));
  for (int index = 0; index < k_indexOfFactorExpression; ++index) {
    m_layouts[index] =
        exactOutput.createLayout(Preferences::PrintFloatMode::Decimal,
                                 Preferences::LargeNumberOfSignificantDigits,
                                 nullptr, baseAtIndex(index));
  }
  // Computing factorExpression
  Expression factor = UserExpression::Create(KFactor(KA), {.KA = exactOutput});
  bool reductionFailure = false;
  PoincareHelpers::CloneAndSimplify(
      &factor, App::app()->localContext(),
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()},
      &reductionFailure);
  if (!reductionFailure && !factor.isUndefined() &&
      !factor.tree()->treeIsIdenticalTo(1_e) &&
      !factor.tree()->treeIsIdenticalTo(0_e)) {
    m_layouts[k_indexOfFactorExpression] =
        PoincareHelpers::CreateLayout(factor, App::app()->localContext());
  }
}

I18n::Message IntegerListController::messageAtIndex(int index) {
  switch (index) {
    case 0:
      return I18n::Message::Decimal;
    case 1:
      return I18n::Message::HexadecimalBase;
    case 2:
      return I18n::Message::BinaryBase;
    default:
      assert(index == 3);
      return I18n::Message::PrimeFactors;
  }
}

}  // namespace Calculation
