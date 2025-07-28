#include <ion/display.h>
#include <ion/storage/file_system.h>

namespace Ion {

void Init() {
  Display::Context::SharedContext.init();
  Storage::FileSystem::sharedFileSystem.init();
}

}  // namespace Ion
