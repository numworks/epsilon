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
  Preferences * preferences = Preferences::sharedPreferences();
  Preferences::PrintFloatMode previousMode = preferences->displayMode();
  preferences->setDisplayMode(Preferences::PrintFloatMode::Scientific);
  Context * context = App::app()->localContext();
  int index = 0;
  Expression value = a.approximate<double>(context, preferences->complexFormat(), preferences->angleUnit());
  if (value.isUndefined()) {
    // Units can't be approximated, we remove them temporarily
    Expression units;
    value = a.removeUnit(&units);
    value = Multiplication::Builder(value.approximate<double>(context, preferences->complexFormat(), preferences->angleUnit()), units);
  }
  m_layouts[index++] = PoincareHelpers::CreateLayout(value, context);
  preferences->setDisplayMode(previousMode);
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}
