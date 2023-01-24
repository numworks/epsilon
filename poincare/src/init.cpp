#include <poincare/init.h>
#include <poincare/preferences.h>
#include <poincare/tree_pool.h>

namespace Poincare {

void Init() {
  Preferences::sharedPreferences.init();
  TreePool::sharedPool.init();
}

}
