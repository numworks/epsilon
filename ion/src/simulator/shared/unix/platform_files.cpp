#include "../platform.h"

namespace Ion {
namespace Simulator {
namespace Platform {

const char * filePathInTempDir(const char * filename) {
  static char path[64];
  if (snprintf(path, sizeof(path), "/tmp/%s", filename) < 0) {
    return nullptr;
  }
  return path;
}

}
}
}
