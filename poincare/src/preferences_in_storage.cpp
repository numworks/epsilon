#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/preferences.h>

namespace Poincare {

void Preferences::Init(Preferences::Interface* preferences) {
  s_preferences = preferences;
}

}  // namespace Poincare
