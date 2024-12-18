#if POINCARE_TREE_STACK_VISUALIZATION
#include <assert.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/visualization.h>

#include <fstream>

namespace Poincare::Internal {

static bool s_forceClosed = false;

std::ofstream& Logger() {
  static std::ofstream s_loggerFile;
  std::ofstream& file = s_loggerFile;
  if (s_forceClosed) {
    file = std::ofstream("/dev/null");
  }
  if (!file.is_open()) {
    std::filesystem::create_directories("./output/logs");
    const char* fileName = "./output/logs/edition.xml";
    file.open(fileName, std::ofstream::out | std::ofstream::trunc);
    file << "<?xml version=\"1.0\"?>\n<Data>\n";
  }
  assert(file.is_open());
  return file;
}

void ResetLogger() { Logger().close(); }

void CloseLogger() {
  Logger() << "</Data>" << std::endl;
  Logger().close();
  s_forceClosed = true;
}

void Log(const char* event, const void* blockAddress, size_t blockSize,
         const void* pointerAddress) {
  Logger() << "  <" << event;
  if (blockAddress) {
    Logger() << " blockAddress=\"" << blockAddress << "\"";
  }
  if (blockSize < INT_MAX) {
    Logger() << " blockSize=\"" << blockSize << "\"";
  }
  if (pointerAddress) {
    Logger() << " pointerAddress=\"" << pointerAddress << "\"";
  }
  Logger() << ">\n";
  /* TODO: This crash when an incomplete tree is being edited. We should either
   * skip log if pool is "broken", or make the log foolproof (logging nodes
   * instead of trees). */
  SharedTreeStack->log(Logger(), TreeStack::LogFormat::Flat, true, 2);
  Logger() << "\n  </" << event << ">" << std::endl;
}

}  // namespace Poincare::Internal

#endif
