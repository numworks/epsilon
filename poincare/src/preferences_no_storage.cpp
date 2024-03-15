#include <assert.h>
#include <poincare/preferences.h>

namespace Poincare {

OMG::GlobalBox<Preferences> s_sharedPreferences;

void Preferences::Init() { s_sharedPreferences.init(); }

Preferences* Preferences::SharedPreferences() { return s_sharedPreferences; }

}  // namespace Poincare
