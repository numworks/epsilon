#include <ion/storage/file_system.h>

namespace Ion {

void Init() {
  Storage::FileSystem::sharedFileSystem.init();
}

}
