#ifndef ION_DEVICE_USERLAND_DRIVERS_PERSISTING_BYTES_H
#define ION_DEVICE_USERLAND_DRIVERS_PERSISTING_BYTES_H

#include <ion/exam_bytes.h>
#include <stdint.h>

namespace Ion {
namespace PersistingBytes {

/* Persisting bytes consist of multiple entries, each with a fixed entrySize.
 * Since flash memory has limited write cycles, each entry is stored in a
 * sector larger than its entrySize. This approach allows us to append new
 * values to the entry instead of overwriting it.
 *
 * The layout of sectors is as follows:
 * |    PersistingBytes Sector (64 kB)     |
 * | ExamBytes (63 kB) | DeviceName (1 kB) |
 *
 * WARNING: UserlandHeader relies on DeviceName being at the end.
 *
 * Entries:
 *  - ExamBytes:
 *    > entrySize = 2 bytes (sizeof(ExamBytes::Int))
 *    > sectorSize = 63 kB
 *      NOTE: The sectorSize is larger than required and can be divided into
 *            smaller sectors if additional entries need to be stored.
 *
 *  - DeviceName:
 *    > entrySize = 1024 bytes
 *    > sectorSize = 1024 bytes
 *      NOTE: The name is only written during device flashing, so it doesn't
 *            need more space for new values.
 */

enum class Entry : uint8_t {
  ExamBytes = 0,
  DeviceName = 1,
  NumberOfEntries = 2
};

constexpr uint8_t k_numberOfEntries =
    static_cast<uint8_t>(Entry::NumberOfEntries);

constexpr uint16_t k_entriesEntrySize[k_numberOfEntries] = {
    sizeof(ExamBytes::Int), 1024};

uint16_t entrySize(Entry entry);

void write(uint8_t* data, uint16_t size, Entry entry);
uint8_t* read(Entry entry);

}  // namespace PersistingBytes
}  // namespace Ion

#endif
