#include "actions.h"
#include "framebuffer.h"
#include "platform.h"
#include "state_file.h"

namespace Ion {
namespace Simulator {
namespace Actions {

constexpr const char * kStateFileExtension = "nws";

void saveState() {
  const char * path = Platform::filePathForWriting(kStateFileExtension);
  if (path != nullptr) {
    StateFile::save(path);
  }
}

#if 0
void loadState() {
// We would need to be able to perform a reset for this to be callable anytime
  const char * path = Platform::filePathForReading(kStateFileExtension);
  if (path != nullptr) {
    StateFile::load(path);
  }
}
#endif

void takeScreenshot() {
  const char * path = Platform::filePathForWriting("png");
  if (path != nullptr) {
//    Framebuffer::writeToFile(path);
  }
}

}
}
}
