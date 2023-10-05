#include "poincare_junior/include/poincare.h"

#include <assert.h>

#include "poincare_junior/src/memory/cache_pool.h"
#include "poincare_junior/src/memory/edition_pool.h"

#if POINCARE_POOL_VISUALIZATION
#include <filesystem>
#endif

namespace PoincareJ {

void Init() { CachePool::SharedCachePool.init(); }

void Shutdown() {
#if POINCARE_POOL_VISUALIZATION
  CloseLogger(LoggerType::Cache);
  CloseLogger(LoggerType::Edition);
#endif
}

#if POINCARE_POOL_VISUALIZATION
static bool s_forceClosed[static_cast<int>(LoggerType::NumberOfLoggers)] = {
    false, false};

std::ofstream& Logger(LoggerType type) {
  static std::ofstream
      s_loggerFiles[static_cast<int>(LoggerType::NumberOfLoggers)];
  int indexOfType = static_cast<int>(type);
  std::ofstream& file = s_loggerFiles[indexOfType];
  if (s_forceClosed[indexOfType]) {
    file = std::ofstream("/dev/null");
  }
  if (!file.is_open()) {
    std::filesystem::create_directories("./output/logs");
    const char* fileName = type == LoggerType::Cache
                               ? "./output/logs/cache.xml"
                               : "./output/logs/edition.xml";
    file.open(fileName, std::ofstream::out | std::ofstream::trunc);
    file << "<?xml version=\"1.0\"?>\n<Data>\n";
  }
  assert(file.is_open());
  return file;
}

void ResetLogger(LoggerType type) { Logger(type).close(); }

void CloseLogger(LoggerType type) {
  Logger(type) << "</Data>" << std::endl;
  Logger(type).close();
  s_forceClosed[static_cast<int>(type)] = true;
}

void Log(LoggerType type, const char* event, const void* blockAddress,
         size_t blockSize, const void* pointerAddress) {
  Logger(type) << "  <" << event;
  if (blockAddress) {
    Logger(type) << " blockAddress=\"" << blockAddress << "\"";
  }
  if (blockSize < INT_MAX) {
    Logger(type) << " blockSize=\"" << blockSize << "\"";
  }
  if (pointerAddress) {
    Logger(type) << " pointerAddress=\"" << pointerAddress << "\"";
  }
  Logger(type) << ">\n";
  Pool* pool;
  Pool::LogFormat format;
  if (type == LoggerType::Cache) {
    pool = CachePool::SharedCachePool;
    format = Pool::LogFormat::Tree;
  } else {
    pool = SharedEditionPool;
    format = Pool::LogFormat::Flat;
  }
  pool->log(Logger(type), format, true, 2);
  Logger(type) << "\n  </" << event << ">" << std::endl;
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
