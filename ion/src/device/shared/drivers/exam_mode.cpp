#include <ion/exam_mode.h>
#include <drivers/config/exam_mode.h>
#include "flash.h"
#include <assert.h>

extern "C" {
  extern char _exam_mode_buffer_start;
  extern char _exam_mode_buffer_end;
}

namespace Ion {
namespace ExamMode {

char ones[Config::ExamModeBufferSize]
  __attribute__((section(".exam_mode_buffer")))
  __attribute__((used))
= {EXAM_BUFFER_CONTENT};

/* The exam mode is written in flash so that it is resilient to resets.
 * We erase the dedicated flash sector (all bits written to 1) and, upon
 * deactivating or activating standard, nosym or Dutch exam mode we write one, two or tree
 * bits to 0. To determine in which exam mode we are, we count the number of
 * leading 0 bits. If it is equal to:
 * - 0[3]: the exam mode is off;
 * - 1[3]: the standard exam mode is activated;
 * - 2[3]: the NoSym exam mode is activated;
 * - 3[3]: the Dutch exam mode is activated;
 * - 4[3]: the NoSymNoText exam mode is activated. */

/* significantExamModeAddress returns the first uint32_t * in the exam mode
 * flash sector that does not point to 0. If this flash sector has only 0s or
 * if it has only one 1, it is erased (to 1) and significantExamModeAddress
 * returns the start of the sector. */

constexpr static size_t numberOfBitsInByte = 8;

// if i = 0b000011101, numberOfBitsAfterLeadingZeroes(i) returns 5
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

uint8_t * SignificantExamModeAddress() {
  uint32_t * persitence_start_32 = (uint32_t *)&_exam_mode_buffer_start;
  uint32_t * persitence_end_32 = (uint32_t *)&_exam_mode_buffer_end;
  assert((persitence_end_32 - persitence_start_32) % 4 == 0);
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
    assert(Ion::Device::Flash::SectorAtAddress((uint32_t)&_exam_mode_buffer_start) >= 0);
    Ion::Device::Flash::EraseSector(Ion::Device::Flash::SectorAtAddress((uint32_t)&_exam_mode_buffer_start));
    return (uint8_t *)&_exam_mode_buffer_start;
  }

  return persitence_start_8;
}

uint8_t FetchExamMode() {
  uint8_t * readingAddress = SignificantExamModeAddress();
  // Count the number of 0[3] before reading address
  uint32_t nbOfZerosBefore = ((readingAddress - (uint8_t *)&_exam_mode_buffer_start) * numberOfBitsInByte) % 4;
  // Count the number of 0[3] at reading address
  size_t numberOfLeading0 = (numberOfBitsInByte - numberOfBitsAfterLeadingZeroes(*readingAddress)) % 4;
  return (nbOfZerosBefore + numberOfLeading0) % 4;
}

void IncrementExamMode(uint8_t delta) {
  assert(delta == 1 || delta == 2 || delta == 3);
  uint8_t * writingAddress = SignificantExamModeAddress();
  assert(*writingAddress != 0);
  size_t nbOfTargetedOnes = numberOfBitsAfterLeadingZeroes(*writingAddress);

  // Compute the new value with delta bits switched to 0.
  /* We write in 2 bytes instead of 1, in case there was only one bit
   * left to 1 in writingAddress. */
  nbOfTargetedOnes += numberOfBitsInByte;
  nbOfTargetedOnes -= delta;
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
  assert(writingAddress < (uint8_t *)&_exam_mode_buffer_end - 1 || (writingAddress == (uint8_t *)&_exam_mode_buffer_end - 1 && writtenFlash == 1));
  Ion::Device::Flash::WriteMemory(writingAddress, newValue, writtenFlash);
}

}
}
