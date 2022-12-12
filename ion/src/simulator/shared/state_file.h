#ifndef ION_SIMULATOR_STATE_FILE_H
#define ION_SIMULATOR_STATE_FILE_H

namespace Ion {
namespace Simulator {
namespace StateFile {

void load(const char* filename);
bool loadMemory(const char* buffer, size_t length);
void save(const char* filename);

}  // namespace StateFile
}  // namespace Simulator
}  // namespace Ion

#endif
