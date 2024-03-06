#include "integer_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/based_integer.h>
#include <poincare/factor.h>
#include <poincare/integer.h>

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
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  static_assert(
      k_maxNumberOfRows >= k_indexOfFactorExpression + 1,
      "k_maxNumberOfRows must be greater than k_indexOfFactorExpression");
  assert(AdditionalResultsType::HasInteger(exactOutput));
#if 0  // TODO_PCJ
  Integer integer = static_cast<const BasedInteger &>(exactOutput).integer();
#else
  assert(false);
  Integer integer;
#endif
  for (int index = 0; index < k_indexOfFactorExpression; ++index) {
    if (baseAtIndex(index) == OMG::Base::Decimal) {
      // TODO PCJ only handle this one yet
      m_layouts[index] = exactOutput.createLayout(
          Preferences::PrintFloatMode::Decimal,
          Preferences::LargeNumberOfSignificantDigits, nullptr);
    } else {
      m_layouts[index] = integer.createLayout(baseAtIndex(index));
    }
  }
  // Computing factorExpression
  Expression factor = Factor::Builder(exactOutput.clone());
  PoincareHelpers::CloneAndSimplify(
      &factor, App::app()->localContext(),
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  if (!factor.isUndefined() && !factor.isIdenticalTo(Rational::Builder(1)) &&
      !factor.isIdenticalTo(Rational::Builder(0))) {
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
