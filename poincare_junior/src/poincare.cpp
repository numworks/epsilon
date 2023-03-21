#include "poincare_junior/include/poincare.h"

#include <assert.h>

#if POINCARE_POOL_VISUALIZATION
#include <filesystem>
#endif

namespace PoincareJ {

void Init() {}

void Shutdown() {
#if POINCARE_POOL_VISUALIZATION
  CloseCacheLogger();
#endif
}

#if POINCARE_POOL_VISUALIZATION
static bool s_forceClosed = false;

std::ofstream& CacheLogger() {
  static std::ofstream s_cacheLogger;
  if (s_forceClosed) {
    s_cacheLogger = std::ofstream("/dev/null");
  }
  if (!s_cacheLogger.is_open()) {
    std::filesystem::create_directories("./output/logs");
    s_cacheLogger.open("./output/logs/cache.xml", std::ofstream::out | std::ofstream::trunc);
    s_cacheLogger << "<?xml version=\"1.0\"?>\n<Data>\n";
  }
  assert(s_cacheLogger.is_open());
  return s_cacheLogger;
}

void ResetCacheLogger() {
  CacheLogger().close();
}

void CloseCacheLogger() {
  CacheLogger() << "</Data>" << std::endl;
  CacheLogger().close();
  s_forceClosed = true;
}

#endif

#if POINCARE_MEMORY_TREE_LOG
void Indent(std::ostream& stream, int indentation) {
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
}
#endif

}  // namespace PoincareJ
