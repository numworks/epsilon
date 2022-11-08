#include "scientific_notation_helper.h"
#include <apps/shared/poincare_helpers.h>
#include "../app.h"

using namespace Poincare;

namespace Calculation {

namespace ScientificNotationHelper {

bool HasAdditionalOutputs(Expression a) {
  Context * context = App::app()->localContext();
  Layout historyResult = Shared::PoincareHelpers::CreateLayout(a, context, Preferences::sharedPreferences());
  return !historyResult.isIdenticalTo(ScientificLayout(a));
}

Layout ScientificLayout(Expression a) {
  assert(!a.hasUnit());
  Preferences preferences = *Preferences::sharedPreferences();
  preferences.setDisplayMode(Preferences::PrintFloatMode::Scientific);
  Context * context = App::app()->localContext();
  return Shared::PoincareHelpers::CreateLayout(a, context, &preferences);
}

}

}
