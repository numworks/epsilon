#include "poincare_junior/include/poincare.h"

#include <assert.h>

#if POINCARE_POOL_VISUALIZATION
#include <filesystem>
#endif

namespace PoincareJ {

void Init() {}

void Shutdown() {
#if POINCARE_POOL_VISUALIZATION
  CacheLogger() << "</Data>" << std::endl;
  CacheLogger().close();
#endif
}

#if POINCARE_POOL_VISUALIZATION
std::ofstream& CacheLogger() {
  static std::ofstream s_cacheLogger;
  if (!s_cacheLogger.is_open()) {
    std::filesystem::create_directories("./output/logs");
    s_cacheLogger.open("./output/logs/cache.log");
    s_cacheLogger << "<?xml version=\"1.0\"?>\n<Data>\nq";
  }
  assert(s_cacheLogger.is_open());
  return s_cacheLogger;
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
