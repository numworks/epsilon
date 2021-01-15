#include "persisting_bytes.h"
#include <drivers/cache.h>
#include <drivers/config/persisting_bytes.h>
#include <drivers/flash.h>
#include <assert.h>

extern "C" {
  extern char _persisting_bytes_buffer_start;
  extern char _persisting_bytes_buffer_end;
}

namespace Ion {
namespace Device {
namespace PersistingBytes {

//TODO LEA add tests
//
//TODO LEA Do we want to persist 1 byte only?

char ones[PersistingBytes::Config::BufferSize]
__attribute__((section(".persisting_bytes_buffer")))
__attribute__((used))
= {PERSISTING_BYTES_BUFFER_CONTENT};

/*
 *  Erased Memory  -> 11111111|11111111|11111111|
 *  Persisted byte -> 01111111|11111111|11111111|
 *                    ^persisted byte
 *                    01111111|11100011|11111111|
 *                             ^persisted byte
 * */

uint8_t * SignificantPersistedByteAddress() {
  uint32_t * persitence_start_32 = reinterpret_cast<uint32_t *>(&_persisting_bytes_buffer_start);
  uint32_t * persitence_end_32 = reinterpret_cast<uint32_t *>(&_persisting_bytes_buffer_end);
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

void write(uint8_t byte) {
  assert(byte != 0xFF); // Unvalid value //TODO LEA, assert or escape?
  uint8_t * writingAddress = SignificantPersistedByteAddress() + 1;

  if (writingAddress == reinterpret_cast<uint8_t *>(&_persisting_bytes_buffer_end - 1)) {
    /* If there are no remaining erased bytes, go back to the beginning of the
     * buffer. */
    int bufferSector = Ion::Device::Flash::SectorAtAddress(reinterpret_cast<uint32_t >(&_persisting_bytes_buffer_start));
    assert(bufferSector == Ion::Device::Flash::SectorAtAddress(reinterpret_cast<uint32_t >(&_persisting_bytes_buffer_end - 1)));
    Ion::Device::Flash::EraseSector(bufferSector);
    writingAddress = reinterpret_cast<uint8_t *>(&_persisting_bytes_buffer_start);
  }

  // Write the value in flash
  Ion::Device::Flash::WriteMemory(writingAddress, &byte, 1);
  Ion::Device::Cache::invalidateDCache();
}

uint8_t read() {
  uint8_t * writingAddress = SignificantPersistedByteAddress();
  return *writingAddress;
}

}
}
}
