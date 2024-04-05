#include <poincare/init.h>
#include <poincare/pool.h>
#include <poincare/preferences.h>

namespace Poincare {

void Init() {
  Preferences::Init();
  Pool::sharedPool.init();
}

}  // namespace Poincare
