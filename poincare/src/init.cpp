#include <poincare/context.h>
#include <poincare/exam_mode.h>
#include <poincare/init.h>
#include <poincare/pool.h>
#include <poincare/preferences.h>
#include <poincare/src/memory/tree_stack.h>

#if POINCARE_TREE_STACK_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare {

void Init(Preferences::Interface* preferences, const ExamMode* examModePtr,
          Poincare::Context* globalContext) {
  Preferences::Init(preferences);
  ExamModeStore::Init(examModePtr);
  Context::Init(globalContext);
  Pool::sharedPool.init();
  Internal::SharedTreeStack.init();
}

void Shutdown() {
#if POINCARE_TREE_STACK_VISUALIZATION
  Internal::CloseLogger();
#endif
}

}  // namespace Poincare
