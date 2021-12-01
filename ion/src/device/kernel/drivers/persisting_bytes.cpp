#include "persisting_bytes.h"
#include <drivers/cache.h>
#include <drivers/flash_privileged.h>
#include <assert.h>
#include <string.h>

extern "C" {
  extern char _persisting_bytes_buffer_start;
  extern char _persisting_bytes_buffer_end;
}

namespace Ion {
namespace Device {
namespace PersistingBytes {

/*  With two persisting bytes :
 *  Erased Memory  -> 11111111|11111111|11111111|11111111|11111111|11111111|
 *  Persisted byte -> 01010101|01111111|11111111|11111111|11111111|11111111|
 *                    ^ SignificantPersistedByteAddress
 *                    01010101|01111111|11111111|01110111|11111111|11111111|
 *                                      ^ SignificantPersistedByteAddress
 * */

uint8_t * SignificantPersistedByteAddress() {
  uint32_t * persistence_start_32 = reinterpret_cast<uint32_t *>(&_persisting_bytes_buffer_start);
  uint32_t * persistence_end_32 = reinterpret_cast<uint32_t *>(&_persisting_bytes_buffer_end);
  // Add 3 so that, with 1,2,3 or 4 persisting bytes, this number is 1.
  uint32_t numberOfPersistingBytes_32 = (k_numberOfPersistingBytes + 3) / 4;
  do {
    // Scan backwards by groups of 4 bytes to reach first non-one uint32_t
    persistence_end_32--;
  } while (persistence_end_32 - numberOfPersistingBytes_32 >= persistence_start_32 && *persistence_end_32 == 0xFFFFFFFF);
  uint8_t * persistence_start_8 = reinterpret_cast<uint8_t *>(persistence_start_32);
  uint8_t * persistence_end_8 = reinterpret_cast<uint8_t *>(persistence_end_32 + 1);
  do {
    // Scan backwards by byte to reach first non-one byte
    persistence_end_8--;
  } while (persistence_end_8 - k_numberOfPersistingBytes >= persistence_start_8 && *persistence_end_8 == 0xFF);
  return persistence_end_8 + 1 - k_numberOfPersistingBytes;
}

/* TODO : Add a write(uint8_t * byteArray, uint8_t size) to avoid multiple
 * unnecessary memmoves when updating multiple bytes at once. */
void write(uint8_t byte, uint8_t index) {
  assert(index < k_numberOfPersistingBytes);
  assert(byte != 0xFF); // Unvalid value // TODO : assert or escape ?
  uint8_t * writingAddress = SignificantPersistedByteAddress();
  assert(writingAddress + k_numberOfPersistingBytes <= reinterpret_cast<uint8_t *>(&_persisting_bytes_buffer_end));
  if ((*(writingAddress + index) & byte) != byte) {
    /* We can't simply override at writingAddress to write the byte, we also
     * need to move every persisting bytes by k_numberOfPersistingBytes. */
    uint8_t buffer[k_numberOfPersistingBytes];
    memcpy(buffer, writingAddress, k_numberOfPersistingBytes);
    writingAddress += k_numberOfPersistingBytes;
    if (writingAddress + k_numberOfPersistingBytes > reinterpret_cast<uint8_t *>(&_persisting_bytes_buffer_end)) {
      /* If there are no remaining erased bytes, go back to the beginning of the
       * buffer. */
      int bufferSector = Ion::Device::Flash::SectorAtAddress(reinterpret_cast<uint32_t >(&_persisting_bytes_buffer_start));
      assert(bufferSector == Ion::Device::Flash::SectorAtAddress(reinterpret_cast<uint32_t >(&_persisting_bytes_buffer_end - 1)));
      Ion::Device::Flash::EraseSector(bufferSector);
      writingAddress = reinterpret_cast<uint8_t *>(&_persisting_bytes_buffer_start);
    }
    // Write the other persisting bytes
    for (size_t i = 0; i < k_numberOfPersistingBytes; i++) {
      if (i != index) {
        Ion::Device::Flash::WriteMemory(writingAddress + i, buffer + i, 1);
      }
    }
  }
  // Write the value in flash
  Ion::Device::Flash::WriteMemory(writingAddress + index, &byte, 1);
  Ion::Device::Cache::invalidateDCache();
}

uint8_t read(uint8_t index) {
  assert(index < k_numberOfPersistingBytes);
  uint8_t * writingAddress = SignificantPersistedByteAddress();
  assert(writingAddress + k_numberOfPersistingBytes <= reinterpret_cast<uint8_t *>(&_persisting_bytes_buffer_end));
  return *(writingAddress + index);
}

}
}
}
