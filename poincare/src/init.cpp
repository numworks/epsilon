#include <poincare/init.h>
#include <poincare/old/pool.h>
#include <poincare/old/preferences.h>

namespace Poincare {

void Init() {
  Preferences::Init();
  Pool::sharedPool.init();
}

}  // namespace Poincare
