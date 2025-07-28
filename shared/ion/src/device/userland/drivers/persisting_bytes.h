#ifndef ION_DEVICE_USERLAND_DRIVERS_PERSISTING_BYTES_H
#define ION_DEVICE_USERLAND_DRIVERS_PERSISTING_BYTES_H

#include <assert.h>
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
 * | DeviceName (1 kB) | ExamBytes (63 kB) |
 *
 * Entries:
 *  - DeviceName:
 *    > entry size = 1024 bytes
 *    > sector size = 1024 bytes
 *      NOTE: The name is only written during device flashing, so it doesn't
 *            need more space for new values.
 *
 *  - ExamBytes:
 *    > entry size = 2 bytes (sizeof(ExamBytes::Int))
 *    > sector size = 63 kB
 *      NOTE: The sectorSize is larger than required and can be divided into
 *            smaller sectors if additional entries need to be stored.
 *
 */

/* When adding a new Entry, just add an element to k_entriesProperties and edit
 * the sectorStart and sectorEnd methods in .cpp*/
enum class Entry : uint8_t { DeviceName = 0, ExamBytes, NumberOfEntries };

constexpr uint8_t k_numberOfEntries =
    static_cast<uint8_t>(Entry::NumberOfEntries);

constexpr struct {
  uint16_t size;              // Max size of one entry
  const void* defaultValue;   // Value the entry takes when sector is init
  uint16_t defaultValueSize;  // Size of the default value
} k_entriesProperties[k_numberOfEntries] = {
    /* DeviceName */
    {.size = 1024, .defaultValue = "", .defaultValueSize = 1},
    /* ExamBytes */
    {.size = sizeof(ExamBytes::Int),
     .defaultValue = &ExamBytes::k_defaultValue,
     .defaultValueSize = sizeof(ExamBytes::Int)},
};

constexpr uint16_t entrySize(Entry entry) {
  assert(entry != Entry::NumberOfEntries);
  return k_entriesProperties[static_cast<uint8_t>(entry)].size;
}

void write(uint8_t* data, uint16_t size, Entry entry);
uint8_t* read(Entry entry);

}  // namespace PersistingBytes
}  // namespace Ion

#endif
