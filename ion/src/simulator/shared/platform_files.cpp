#include "platform.h"

namespace Ion {
namespace Simulator {
namespace Platform {

const char* cacheWindowPositionFilePath() {
  return filePathInTempDir("numworks.pos");
}

}  // namespace Platform
}  // namespace Simulator
}  // namespace Ion