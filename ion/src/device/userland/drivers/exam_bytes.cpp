#include <assert.h>
#include <ion/exam_bytes.h>
#include <shared/drivers/flash_information.h>
#include <shared/drivers/flash_write_with_interruptions.h>
#include <string.h>

#include "persisting_bytes.h"

namespace Ion {
namespace ExamBytes {

/*  With three persisting bytes :
 *  Erased Memory  -> 11111111|11111111|11111111|11111111|11111111|11111111|
 *  Persisted byte -> 01010101|01111111|11111111|11111111|11111111|11111111|
 *                    ^ SignificantPersistedByteAddress
 *                    01010101|01111111|11111111|01110111|11111111|11111111|
 *                                      ^ SignificantPersistedByteAddress
 * */

Int *SignificantByteAddress() {
  uint32_t *persistence_start_32 =
      reinterpret_cast<uint32_t *>(Ion::PersistingBytes::bufferStart());
  uint32_t *persistence_end_32 =
      reinterpret_cast<uint32_t *>(Ion::PersistingBytes::bufferEnd());
  do {
    // Scan backwards by groups of 4 bytes to reach first non-one uint32_t
    persistence_end_32--;
  } while (persistence_end_32 >= persistence_start_32 &&
           *persistence_end_32 == 0xFFFFFFFF);

  Int *persistence_start_int = reinterpret_cast<Int *>(persistence_start_32);
  Int *persistence_end_int = reinterpret_cast<Int *>(persistence_end_32 + 1);
  do {
    persistence_end_int--;
  } while (persistence_end_int >= persistence_start_int &&
           *persistence_end_int == static_cast<Int>(-1));
  return persistence_end_int;
}

Int read() { return *SignificantByteAddress(); }

void write(Int value) {
  Int *writingAddress = SignificantByteAddress();
  /* If writing the value does not require to change 0s into 1s, we can
   * overwrite the previous value */
  if ((value & ~*writingAddress) != 0) {
    writingAddress += 1;
    if (writingAddress >=
        reinterpret_cast<Int *>(Ion::PersistingBytes::bufferEnd())) {
      /* If there are no remaining erased bytes, go back to the beginning of the
       * buffer. */
      int bufferSector = Ion::Device::Flash::SectorAtAddress(
          reinterpret_cast<uint32_t>(Ion::PersistingBytes::bufferStart()));
      assert(bufferSector ==
             Ion::Device::Flash::SectorAtAddress(reinterpret_cast<uint32_t>(
                 Ion::PersistingBytes::bufferEnd() - 1)));
      Ion::Device::Flash::EraseSectorWithInterruptions(bufferSector, true);
      writingAddress =
          reinterpret_cast<Int *>(Ion::PersistingBytes::bufferStart());
    }
  }
  Ion::Device::Flash::WriteMemoryWithInterruptions(
      reinterpret_cast<uint8_t *>(writingAddress),
      reinterpret_cast<uint8_t *>(&value), sizeof(value), true);
}

}  // namespace ExamBytes
}  // namespace Ion
