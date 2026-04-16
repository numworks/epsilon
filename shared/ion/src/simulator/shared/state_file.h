#pragma once

#include <stdlib.h>

namespace Ion {
namespace Simulator {
namespace StateFile {

void load(const char* filename, bool headlessStateFile = false);
void pushEventFromMemory(uint8_t c, const uint8_t** ptr,
                         const uint8_t* bufferEnd);
bool loadMemory(const char* buffer, size_t length,
                bool headlessStateFiles = false);
#if ION_SIMULATOR_FILES
void save(const char* filename, bool toText = false);
#endif

}  // namespace StateFile
}  // namespace Simulator
}  // namespace Ion
