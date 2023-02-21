#include "../shared/platform.h"

namespace Ion {
namespace Simulator {
namespace Platform {

const char* filePathForReading(const char* extension) { return nullptr; }

const char* filePathForWriting(const char* extension) {
  static char path[64];
  if (snprintf(path, sizeof(path), "/tmp/epsilon.%s", extension) < 0) {
    return nullptr;
  }
  return path;
}

}  // namespace Platform
}  // namespace Simulator
}  // namespace Ion
