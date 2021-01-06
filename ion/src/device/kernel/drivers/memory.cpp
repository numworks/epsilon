#include "memory.h"
#include <drivers/cache.h>
#include <drivers/flash.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace Memory {

//TODO LEA add tests
//
//TODO LEA Do we want to persist 1 byte only?

/* TODO EMILIE these pointers should be retrieved at linking or this code should
 *  be done in External flash (only the Ion::Device::Flash::WriteMemory should
 *  be done in internal flash)... However, we need FetchExamMode in Updatable bootloader and in main Epsilon. The info of where is the exam mode should be shared between both in a smart way? */

constexpr static const uint32_t _persisted_byte_buffer_start_address = 0x9000c028;
constexpr static const uint32_t _persisted_byte_buffer_end_address = 0x9000d028;

/*
 *  Erased Memory  -> 11111111|11111111|11111111|
 *  Persisted byte -> 01111111|11111111|11111111|
 *                    ^persisted byte
 *                    01111111|11100011|11111111|
 *                             ^persisted byte
 * */

uint8_t * SignificantPersistedByteAddress() {
  uint32_t * persitence_start_32 = reinterpret_cast<uint32_t *>(_persisted_byte_buffer_start_address);
  uint32_t * persitence_end_32 = reinterpret_cast<uint32_t *>(_persisted_byte_buffer_end_address);
  while ((persitence_start_32 + 1) < persitence_end_32 && *(persitence_start_32 + 1) != 0xFFFFFFFF) {
    // Scan by groups of 4 bytes to reach last non-one uint32_t
    persitence_start_32++;
  }
  uint8_t * persitence_start_8 = reinterpret_cast<uint8_t *>(persitence_start_32);
  uint8_t * persitence_end_8 = reinterpret_cast<uint8_t *>(persitence_end_32);
  while ((persitence_start_8 + 1) < persitence_end_8 && *(persitence_start_8 + 1) != 0xFF) {
    // Scan by byte to reach last non-one byte
    persitence_start_8++;
  }
  return persitence_start_8;
}

/* TODO LEA: optimize writing -> overwrite on the current byte if possible,
 * instead of always using a new byte. */

void PersistByte(uint8_t byte) {
  assert(byte != 0xFF); // Unvalid value //TODO LEA, assert or escape?
  uint8_t * writingAddress = SignificantPersistedByteAddress();

  if (writingAddress == reinterpret_cast<uint8_t *>(_persisted_byte_buffer_end_address - 1)) {
    /* If there are no remaining erased bytes, go back to the beginning of the
     * buffer. */
    int bufferSector = Ion::Device::Flash::SectorAtAddress(_persisted_byte_buffer_start_address);
    assert(bufferSector == Ion::Device::Flash::SectorAtAddress(_persisted_byte_buffer_end_address - 1));
    Ion::Device::Flash::EraseSector(bufferSector);
    writingAddress = reinterpret_cast<uint8_t *>(_persisted_byte_buffer_start_address);
  }

  // Write the value in flash
  Ion::Device::Flash::WriteMemory(writingAddress, &byte, 1);
  Ion::Device::Cache::invalidateDCache();
}

uint8_t PersistedByte() {
  uint8_t * writingAddress = SignificantPersistedByteAddress();
  return *writingAddress;
}

}
}
}
