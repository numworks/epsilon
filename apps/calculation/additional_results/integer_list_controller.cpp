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
  assert(HasIntegerAdditionalResults(exactOutput));
  Integer integer = exactOutput.convert<BasedInteger>().integer();
  for (int index = 0; index < k_indexOfFactorExpression; ++index) {
    m_layouts[index] = integer.createLayout(baseAtIndex(index));
  }
  // Computing factorExpression
  Expression factor = Factor::Builder(exactOutput.clone());
  PoincareHelpers::CloneAndSimplify(&factor, App::app()->localContext(),
                                    ReductionTarget::User);
  if (!factor.isUndefined()) {
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
