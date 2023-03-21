#ifndef POINCAREJ_POINCARE_H
#define POINCAREJ_POINCARE_H

#if POINCARE_POOL_VISUALIZATION
#include <fstream>
#endif

namespace PoincareJ {

void Init();
void Shutdown();

#if POINCARE_POOL_VISUALIZATION
std::ofstream& CacheLogger();
__attribute__((__used__)) void ResetCacheLogger();
__attribute__((__used__)) void CloseCacheLogger();
#endif

#if POINCARE_MEMORY_TREE_LOG
void Indent(std::ostream& stream, int indentation);
#endif

}  // namespace PoincareJ

#endif
