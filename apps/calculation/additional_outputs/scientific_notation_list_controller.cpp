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

bool ScientificNotationListController::HasAdditionalOutputs(Poincare::Expression a) {
  Context * context = App::app()->localContext();
  Layout historyResult = PoincareHelpers::CreateLayout(a, context, Preferences::sharedPreferences());
  return !historyResult.isIdenticalTo(ScientificLayout(a));
}

void ScientificNotationListController::setExactAndApproximateExpression(Poincare::Expression e, Poincare::Expression a) {
  assert(HasAdditionalOutputs(a));
  m_layouts[0] = ScientificLayout(a);
}

Layout ScientificNotationListController::ScientificLayout(Poincare::Expression a) {
  assert(!a.hasUnit());
  Preferences preferences = *Preferences::sharedPreferences();
  preferences.setDisplayMode(Preferences::PrintFloatMode::Scientific);
  Context * context = App::app()->localContext();
  return PoincareHelpers::CreateLayout(a, context, &preferences);
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}
