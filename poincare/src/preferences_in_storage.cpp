#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/preferences.h>

namespace Poincare {

void Preferences::Init(Preferences::PartialInterface* partial) {
  s_preferences = partial;
}

}  // namespace Poincare
