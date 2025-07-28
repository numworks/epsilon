#ifndef ION_SIMULATOR_STATE_FILE_H
#define ION_SIMULATOR_STATE_FILE_H

namespace Ion {
namespace Simulator {
namespace StateFile {

void load(const char* filename, bool headlessStateFile = false);
bool loadMemory(const char* buffer, size_t length,
                bool headlessStateFiles = false);
void save(const char* filename);

}  // namespace StateFile
}  // namespace Simulator
}  // namespace Ion

#endif
