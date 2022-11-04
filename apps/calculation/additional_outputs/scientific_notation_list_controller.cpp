#include "scientific_notation_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/based_integer.h>
#include <poincare/multiplication.h>
#include <poincare/integer.h>
#include <string.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void ScientificNotationListController::setExactAndApproximateExpression(Poincare::Expression e, Poincare::Expression a) {
  Preferences preferences = *Preferences::sharedPreferences();
  preferences.setDisplayMode(Preferences::PrintFloatMode::Scientific);
  Context * context = App::app()->localContext();
  Layout historyResult = PoincareHelpers::CreateLayout(a, context, Preferences::sharedPreferences());
  Expression value = PoincareHelpers::Approximate<double>(e, context, &preferences);
  if (value.isUndefined()) {
    // Units can't be approximated, we remove them temporarily
    Expression units;
    value = a.removeUnit(&units);
    value = PoincareHelpers::Approximate<double>(value, context, &preferences);
    value = Multiplication::Builder(value, units);
  }
  Layout scientific = PoincareHelpers::CreateLayout(value, context, &preferences);
  if (!historyResult.isIdenticalTo(scientific)) {
    m_layouts[0] = scientific;
  }
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}
