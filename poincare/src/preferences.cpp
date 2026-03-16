#include <assert.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/projection.h>

namespace Poincare {

Preferences::Interface* Preferences::s_preferences = nullptr;
const Preferences Preferences::PreferencesInstance;

}  // namespace Poincare
