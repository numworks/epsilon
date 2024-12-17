#ifndef POINCARE_MEMORY_VISUALIZATION_H
#define POINCARE_MEMORY_VISUALIZATION_H

#include <limits.h>

namespace Poincare::Internal {

#if POINCARE_TREE_STACK_VISUALIZATION
// See build/poincare_visualization
__attribute__((__used__)) void ResetLogger();
__attribute__((__used__)) void CloseLogger();
void Log(const char* event, const void* blockAddress = nullptr,
         size_t blockSize = INT_MAX, const void* pointerAddress = nullptr);
#endif

}  // namespace Poincare::Internal

#endif
