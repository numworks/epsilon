#ifndef EXTERNAL_ARCHIVE_H
#define EXTERNAL_ARCHIVE_H

#include <stddef.h>
#include <stdint.h>

namespace External {
namespace Archive {

constexpr int MaxNameLength = 40;

struct File {
    const char *name;
    const uint8_t *data;
    size_t dataLength;
    bool isExecutable;
    bool readable;
};

bool fileAtIndex(size_t index, File &entry);
int indexFromName(const char *name);
size_t numberOfFiles();
size_t numberOfExecutables();
bool executableAtIndex(size_t index, File &entry);
uint32_t executeFile(const char *name, void * heap, const uint32_t heapSize);

}
}

#endif
