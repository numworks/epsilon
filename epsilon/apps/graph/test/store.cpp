#include <quiz.h>
#include <shared/continuous_function_store.h>

#include "helper.h"

namespace Graph {

QUIZ_CASE(graph_continuous_function_store) {
  Shared::ContinuousFunctionStore& functionStore =
      Shared::GlobalContextAccessor::ContinuousFunctionStore();
  const char* definition = "y=x";

  quiz_assert(functionStore.numberOfActiveFunctions() == 0);

  AddFunction(definition);
  [[maybe_unused]] uint32_t checksum =
      Ion::Storage::FileSystem::sharedFileSystem->checksum();
  quiz_assert(functionStore.numberOfActiveFunctions() == 1);

  functionStore.removeAll();
  quiz_assert(functionStore.numberOfActiveFunctions() == 0);

  AddFunction(definition);
  /* Checksum is in practice always different because the Expression identifier
   * and refCounter are uselessly stored in the file system.
   * Uncomment it if it ever changes. */
#if 0
  assert(checksum == Ion::Storage::FileSystem::sharedFileSystem->checksum());
#endif
  quiz_assert(functionStore.numberOfActiveFunctions() == 1);
}

}  // namespace Graph
