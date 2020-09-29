#include "exam_mode_privileged.h"
#include "flash.h"
#include <drivers/cache.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExamMode {

/* TODO EMILIE these pointers should be retrieved at linking or this code should
 *  be done in External flash (only the Ion::Device::Flash::WriteMemory should
 *  be done in internal flash)... However, due to bug in the svcall_handler (to
 *  pass arguments), we do this here for now. */
constexpr static const uint32_t _exam_mode_buffer_start_address = 0x9000c028;
constexpr static const uint32_t _exam_mode_buffer_end_address = 0x9000d028;

/* The exam mode is written in flash so that it is resilient to resets.
 * We erase the dedicated flash sector (all bits written to 1) and, upon
 * deactivating or activating standard or Dutch exam mode we write one or two
 * bits to 0. To determine in which exam mode we are, we count the number of
 * leading 0 bits. If it is equal to:
 * - 0[3]: the exam mode is off;
 * - 1[3]: the standard exam mode is activated;
 * - 2[3]: the Dutch exam mode is activated. */

/* significantExamModeAddress returns the first uint32_t * in the exam mode
 * flash sector that does not point to 0. If this flash sector has only 0s or
 * if it has only one 1, it is erased (to 1) and significantExamModeAddress
 * returns the start of the sector. */

// TODO EMILIE Duplicata - see previous comment TODO EMILIE
constexpr static size_t numberOfBitsInByte = 8;

// TODO EMILIE Duplicata - see previous comment TODO EMILIE
// if i = 0b000011101, firstOneBitInByte(i) returns 5
size_t numberOfBitsAfterLeadingZeroes(int i) {
  int minShift = 0;
  int maxShift = numberOfBitsInByte;
  while (maxShift > minShift+1) {
    int shift = (minShift + maxShift)/2;
    int shifted = i >> shift;
    if (shifted == 0) {
      maxShift = shift;
    } else {
      minShift = shift;
    }
  }
  return maxShift;
}

// TODO EMILIE Duplicata - see previous comment TODO EMILIE
uint8_t * SignificantExamModeAddress() {
  uint32_t * persitence_start_32 = (uint32_t *)_exam_mode_buffer_start_address;
  uint32_t * persitence_end_32 = (uint32_t *)_exam_mode_buffer_end_address;
  while (persitence_start_32 < persitence_end_32 && *persitence_start_32 == 0x0) {
    // Scan by groups of 32 bits to reach first non-zero bit
    persitence_start_32++;
  }
  uint8_t * persitence_start_8 = (uint8_t *)persitence_start_32;
  uint8_t * persitence_end_8 = (uint8_t *)persitence_end_32;
  while (persitence_start_8 < persitence_end_8 && *persitence_start_8 == 0x0) {
    // Scan by groups of 8 bits to reach first non-zero bit
    persitence_start_8++;
  }
  if (persitence_start_8 == persitence_end_8
  // we can't toggle from 0[3] to 2[3] when there is only one 1 bit in the whole sector
  || (persitence_start_8 + 1 == persitence_end_8 && *persitence_start_8 == 1)) {
    assert(Ion::Device::Flash::SectorAtAddress(_exam_mode_buffer_start_address) >= 0);
    Ion::Device::Flash::EraseSector(Ion::Device::Flash::SectorAtAddress(_exam_mode_buffer_start_address));
    return (uint8_t *)_exam_mode_buffer_start_address;
  }

  return persitence_start_8;
}

void ToggleExamMode() {
  uint8_t * writingAddress = SignificantExamModeAddress();
  assert(*writingAddress != 0);
  size_t nbOfTargetedOnes = numberOfBitsAfterLeadingZeroes(*writingAddress);

  // Compute the new value with delta bits switched to 0.
  /* We write in 2 bytes instead of 1, in case there was only one bit
   * left to 1 in writingAddress. */
  nbOfTargetedOnes += numberOfBitsInByte;
  nbOfTargetedOnes -= 1;
  constexpr size_t newValueSize = sizeof(uint16_t)/sizeof(uint8_t);
  uint8_t newValue[newValueSize];
  if (nbOfTargetedOnes > numberOfBitsInByte) {
    size_t nbOfTargetedOnesInFirstByte = nbOfTargetedOnes - numberOfBitsInByte;
    assert(nbOfTargetedOnesInFirstByte <= numberOfBitsInByte);
    newValue[0] = ((uint16_t)1 << nbOfTargetedOnesInFirstByte) - 1;
    newValue[1] = 0xFF;
  } else {
    assert(nbOfTargetedOnes <= numberOfBitsInByte);
    newValue[0] = 0;
    newValue[1] = ((uint16_t)1 << nbOfTargetedOnes) - 1;
  }

  // Write the value in flash
  /* As the number of changed bits is capped by 2, if *writingAddress has more
   * than one remaining 1 bit, we know we toggle bits only in the first byte of
   * newValue. We can settle for writing one byte instead of two. */
  size_t writtenFlash = *writingAddress == 1 ? sizeof(uint16_t) : sizeof(uint8_t);
  /* Avoid writing out of sector */
  //assert(writingAddress < _exam_mode_buffer_end_address - 1 || (writingAddress == _exam_mode_buffer_end_address - 1 && writtenFlash == 1));
  Ion::Device::Flash::WriteMemory(writingAddress, newValue, writtenFlash);
  Ion::Device::Cache::invalidateDCache();
}

}
}
}
