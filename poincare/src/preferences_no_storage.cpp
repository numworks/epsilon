#include <assert.h>
#include <poincare/preferences.h>

namespace Poincare {

OMG::GlobalBox<Preferences> s_SharedPreferences;

void Preferences::Init() { s_SharedPreferences.init(); }

Preferences* Preferences::SharedPreferences() { return s_SharedPreferences; }

}  // namespace Poincare
