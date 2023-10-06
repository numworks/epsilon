#include <assert.h>
#include <drivers/persisting_bytes.h>
#include <drivers/svcall.h>
#include <shared/drivers/flash_information.h>
#include <shared/drivers/flash_write_with_interruptions.h>
#include <string.h>

extern "C" {
extern char _persisting_bytes_buffer_start;
extern char _persisting_bytes_buffer_end;
}

namespace Ion {
namespace PersistingBytes {

uint8_t* BufferStart() {
  return reinterpret_cast<uint8_t*>(&_persisting_bytes_buffer_start);
}

uint8_t* BufferEnd() {
  return reinterpret_cast<uint8_t*>(&_persisting_bytes_buffer_end);
}

uint16_t entrySize(Entry entry) {
  assert(entry != Entry::NumberOfEntries);
  uint16_t size = k_entriesEntrySize[static_cast<uint8_t>(entry)];
  assert(size <= sectorSize(entry) && size > 0);
  return size;
}

uint16_t sectorSize(Entry entry) {
  uint16_t size = 0;
  switch (entry) {
    case Entry::ExamBytes:
      size = BufferEnd() - BufferStart() - sectorSize(Entry::DeviceName);
      break;
    default:
      assert(entry == Entry::DeviceName);
      size = entrySize(Entry::DeviceName);
  }
  assert(size <= BufferEnd() - BufferStart() && size > 0);
  return size;
}

uint8_t* sectorStart(Entry entry) {
  uint8_t* start = nullptr;
  switch (entry) {
    case Entry::ExamBytes:
      start = BufferStart();
      break;
    default:
      assert(entry == Entry::DeviceName);
      start = BufferEnd() - sectorSize(Entry::DeviceName);
  }
  assert(start && start >= BufferStart() &&
         start + sectorSize(entry) <= BufferEnd());
  return start;
}

void initEntry(Entry entry) {
  // Init entry to 0
  uint16_t eSize = entrySize(entry);
  uint8_t* sStart = sectorStart(entry);
  constexpr uint8_t k_zero = 0;
  for (int i = 0; i < eSize; i++) {
    Device::Flash::WriteMemoryWithInterruptions(sStart + i, &k_zero, 1, true);
  }
}

/* Example of writing persisting bytes in the ExamBytes sector (using a
 * sectorSize of 6 bytes):
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
  uint16_t sSize = sectorSize(entry);
  uint8_t* sEnd = sStart + sSize;

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

    /* Store values of other entries
     * TODO: These arrays initializations not very clean. */
    uint8_t
        deviceName[k_entriesEntrySize[static_cast<uint8_t>(Entry::DeviceName)]];
    uint8_t
        examBytes[k_entriesEntrySize[static_cast<uint8_t>(Entry::ExamBytes)]];
    uint8_t* savedValues[k_numberOfEntries] = {examBytes, deviceName};

    for (uint8_t s = 0; s < k_numberOfEntries; s++) {
      Entry currentEntry = static_cast<Entry>(s);
      if (currentEntry != entry) {
        memcpy(savedValues[s], read(currentEntry), entrySize(currentEntry));
      }
    }
    /* Erase persisting bytes. */
    Device::Flash::EraseSectorWithInterruptions(persistingBytesSectorIndex,
                                                true);

    /* Rewrite values of other entries. */
    for (uint8_t s = 0; s < k_numberOfEntries; s++) {
      Entry currentEntry = static_cast<Entry>(s);
      if (currentEntry != entry) {
        Device::Flash::WriteMemoryWithInterruptions(
            sectorStart(currentEntry), savedValues[s], entrySize(currentEntry),
            true);
      }
    }
    writingAddress = sStart;
  }
  /* Write the new value. */
  Device::Flash::WriteMemoryWithInterruptions(writingAddress, data, size, true);
}

uint8_t* read(Entry entry) {
  uint8_t* sStart = sectorStart(entry);
  uint16_t sSize = sectorSize(entry);
  uint8_t* sEnd = sStart + sSize;
  uint16_t eSize = entrySize(entry);

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
   * with 1s. This occurs when the value is read for the first time  after
   * flashing the device, and it hasn't been initialized during the flashing
   * procedure. The entry needs to be initialized with 0s.
   * */
  initEntry(entry);
  return sStart;
}

}  // namespace PersistingBytes
}  // namespace Ion
