#include "persisting_bytes.h"

#include <assert.h>
#include <shared/drivers/flash_information.h>
#include <shared/drivers/flash_write_with_interruptions.h>
#include <string.h>

#include <algorithm>

#include "svcall.h"

extern "C" {
extern char _persisting_bytes_buffer_start;
extern char _persisting_bytes_buffer_end;
extern char _device_name_sector_start;
extern char _device_name_sector_end;
extern char _exam_bytes_sector_start;
extern char _exam_bytes_sector_end;
}

namespace Ion {
namespace PersistingBytes {

uint8_t* BufferStart() {
  return reinterpret_cast<uint8_t*>(&_persisting_bytes_buffer_start);
}

uint8_t* BufferEnd() {
  return reinterpret_cast<uint8_t*>(&_persisting_bytes_buffer_end);
}

uint8_t* sectorStart(Entry entry) {
  switch (entry) {
    case Entry::DeviceName:
      return reinterpret_cast<uint8_t*>(&_device_name_sector_start);
    default:
      assert(entry == Entry::ExamBytes);
      return reinterpret_cast<uint8_t*>(&_exam_bytes_sector_start);
  }
}

uint8_t* sectorEnd(Entry entry) {
  switch (entry) {
    case Entry::DeviceName:
      return reinterpret_cast<uint8_t*>(&_device_name_sector_end);
    default:
      assert(entry == Entry::ExamBytes);
      return reinterpret_cast<uint8_t*>(&_exam_bytes_sector_end);
  }
}

constexpr static uint16_t maxEntrySize() {
  uint16_t result = 0;
  for (uint8_t i = 0; i < k_numberOfEntries; i++) {
    result = std::max(result, entrySize(static_cast<Entry>(i)));
  }
  return result;
}

void initEntry(Entry entry) {
  // Init entry to default value
  const uint8_t* entryDefaultValue = reinterpret_cast<const uint8_t*>(
      k_entriesProperties[static_cast<int>(entry)].defaultValue);
  uint16_t entryDefaultValueSize =
      k_entriesProperties[static_cast<int>(entry)].defaultValueSize;
  assert(entryDefaultValue && entryDefaultValueSize > 0 &&
         entryDefaultValueSize <= entrySize(entry));

  /* Copy the defaultValue in a buffer to avoid reading and writing external
   * flash at the same time (which would crash). */
  uint8_t initBuffer[maxEntrySize()];
  memcpy(initBuffer, entryDefaultValue, entryDefaultValueSize);

  Device::Flash::WriteMemoryWithInterruptions(sectorStart(entry), initBuffer,
                                              entryDefaultValueSize, true);
}

constexpr static uint16_t sumOfEntrySizes() {
  uint16_t result = 0;
  for (uint8_t i = 0; i < k_numberOfEntries; i++) {
    result += entrySize(static_cast<Entry>(i));
  }
  return result;
}

/* Example of writing persisting bytes in the ExamBytes sector (using a
 * sector size of 6 bytes):
 *
 * - Blank memory
 *   > |11111111|11111111|11111111|11111111|11111111|11111111|
 * - Write value 00001111 01111110
 *   > |00001111|01111110|11111111|11111111|11111111|11111111|
 *     ^--current value--^
 * - Write value 00000000 01111110
 *   > Since bits can transition from 1 to 0, the new value overwrites the old.
 *   > |00000000|01111110|11111111|11111111|11111111|11111111|
 *     ^--current value--^
 * - Write value 11110000 10000001
 *   > Since bits can't transition from 0 to 1, the new value is appended.
 *   > |00000000|01111110|11110000|10000001|11111111|11111111|
 *                       ^--current value--^
 */
void write(uint8_t* data, uint16_t size, Entry entry) {
  uint16_t eSize = entrySize(entry);
  assert(data && size <= eSize);

  uint8_t* sStart = sectorStart(entry);
  uint8_t* sEnd = sectorEnd(entry);
  assert(eSize <= sEnd - sStart);

  uint8_t* writingAddress = read(entry);
  /* If writing the value does not require to change 0s into 1s, we can
   * overwrite the previous value */
  for (uint16_t offset = 0; offset < size; offset++) {
    if ((*(data + offset) & ~*(writingAddress + offset)) != 0) {
      /* The value needs to be appended. */
      writingAddress += eSize;
      break;
    }
  }
  if (writingAddress >= sEnd) {
    /* When there are no more writable bytes available within the entry sector,
     * it becomes necessary to erase the entire sector and start over from the
     * beginning. The problem is that the persisting bytes can only be erased
     * all at once. Consequently, we must preserve the current values of other
     * entries to rewrite them back into the sector after the erasure. */
    int persistingBytesSectorIndex = Device::Flash::SectorAtAddress(
        reinterpret_cast<uint32_t>(BufferStart()));
    assert(persistingBytesSectorIndex ==
           Device::Flash::SectorAtAddress(
               reinterpret_cast<uint32_t>(BufferEnd() - 1)));

    /* Store values of other entries */
    uint8_t savedValues[sumOfEntrySizes()];
    uint16_t savedValueOffset = 0;
    for (uint8_t s = 0; s < k_numberOfEntries; s++) {
      Entry currentEntry = static_cast<Entry>(s);
      if (currentEntry == entry) {
        continue;
      }
      uint16_t currentEntrySize = entrySize(currentEntry);
      memcpy(savedValues + savedValueOffset, read(currentEntry),
             currentEntrySize);
      savedValueOffset += currentEntrySize;
      assert(savedValueOffset <= sumOfEntrySizes());
    }
    /* Erase persisting bytes. */
    Device::Flash::EraseSectorWithInterruptions(persistingBytesSectorIndex,
                                                true);

    /* Rewrite values of other entries. */
    savedValueOffset = 0;
    for (uint8_t s = 0; s < k_numberOfEntries; s++) {
      Entry currentEntry = static_cast<Entry>(s);
      if (currentEntry == entry) {
        continue;
      }
      uint16_t currentEntrySize = entrySize(currentEntry);
      Device::Flash::WriteMemoryWithInterruptions(
          sectorStart(currentEntry), savedValues + savedValueOffset,
          currentEntrySize, true);
      savedValueOffset += currentEntrySize;
      assert(savedValueOffset <= sumOfEntrySizes());
    }

    writingAddress = sStart;
  }
  /* Write the new value. */
  Device::Flash::WriteMemoryWithInterruptions(writingAddress, data, size, true);
}

uint8_t* read(Entry entry) {
  uint8_t* sStart = sectorStart(entry);
  uint8_t* sEnd = sectorEnd(entry);
  uint16_t eSize = entrySize(entry);
  assert(eSize <= sEnd - sStart);

  // Scan backwards by groups of eSize bytes to reach first non 0xFFF..FF value
  uint8_t* significantBytesStart = sEnd - eSize;
  while (significantBytesStart >= sStart) {
    for (uint16_t offset = 0; offset < eSize; offset++) {
      if (*(significantBytesStart + offset) != 0xFF) {
        return significantBytesStart;
      }
    }
    significantBytesStart -= eSize;
  }

  /* When reaching this point, it indicates that the entire sector is filled
   * with 1s. This occurs when the value is read for the first time after
   * flashing the device, and it hasn't been initialized during the flashing
   * procedure. The entry needs to be initialized.
   * */
  initEntry(entry);
  return sStart;
}

}  // namespace PersistingBytes
}  // namespace Ion
