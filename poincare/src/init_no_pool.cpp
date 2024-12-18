#include <poincare/init.h>
#include <poincare/preferences.h>
#include <poincare/src/memory/tree_stack.h>

#if POINCARE_TREE_STACK_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare {

void Init() {
  Preferences::Init();
  Internal::SharedTreeStack.init();
}

void Shutdown() {
#if POINCARE_TREE_STACK_VISUALIZATION
  Internal::CloseLogger();
#endif
}

}  // namespace Poincare
