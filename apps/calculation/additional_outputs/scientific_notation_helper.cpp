#include "scientific_notation_helper.h"
#include <apps/shared/poincare_helpers.h>

using namespace Poincare;

namespace Calculation {

namespace ScientificNotationHelper {

bool HasAdditionalOutputs(Expression a, Context * context) {
  Layout historyResult = Shared::PoincareHelpers::CreateLayout(a, context, Preferences::sharedPreferences());
  return !historyResult.isIdenticalTo(ScientificLayout(a, context));
}

Layout ScientificLayout(Expression a, Context * context) {
  assert(!a.hasUnit());
  Preferences preferences = *Preferences::sharedPreferences();
  preferences.setDisplayMode(Preferences::PrintFloatMode::Scientific);
  return Shared::PoincareHelpers::CreateLayout(a, context, &preferences);
}

}

}
