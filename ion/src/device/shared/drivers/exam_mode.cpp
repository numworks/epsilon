#include <ion/exam_mode.h>
#include <drivers/config/exam_mode.h>
#include "flash.h"
#include <assert.h>

namespace Ion {
namespace ExamMode {

extern "C" {
  extern char _exam_mode_buffer_start;
  extern char _exam_mode_buffer_end;
}

char ones[Config::ExamModeBufferSize]
  __attribute__((section(".exam_mode_buffer")))
  __attribute__((used))
= {EXAM_BUFFER_CONTENT};

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

uint32_t * SignificantExamModeAddress() {
  uint32_t * persitence_start = (uint32_t *)&_exam_mode_buffer_start;
  uint32_t * persitence_end = (uint32_t *)&_exam_mode_buffer_end;
  while (persitence_start < persitence_end && *persitence_start == 0x0) {
    // Skip even number of zero bits
    persitence_start++;
  }
  if (persitence_start == persitence_end
  // we can't toggle from 0[3] to 2[3] when there is only one 1 bit in the whole sector
  || (persitence_start + 1 == persitence_end && *persitence_start == 1)) {
    assert(Ion::Device::Flash::SectorAtAddress((uint32_t)&_exam_mode_buffer_start) >= 0);
    Ion::Device::Flash::EraseSector(Ion::Device::Flash::SectorAtAddress((uint32_t)&_exam_mode_buffer_start));
    return (uint32_t *)&_exam_mode_buffer_start;
  }
  return persitence_start;
}

size_t firstOneBit(int i, size_t size) {
  int minShift = 0;
  int maxShift = size;
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

uint8_t FetchExamMode() {
  uint32_t * readingAddress = SignificantExamModeAddress();
  // Count the number of 0[3] before reading address
  uint32_t nbOfZerosBefore = ((readingAddress - (uint32_t *)&_exam_mode_buffer_start)/4 * 2) % 3;
  // Count the number of 0[3] at reading address
  size_t numberOfLeading0 = (32 - firstOneBit(*readingAddress, 32)) % 3;
  return (nbOfZerosBefore + numberOfLeading0) % 3;
}

void IncrementExamMode(uint8_t delta) {
  assert(delta == 1 || delta == 2);
  uint32_t * writingAddress = SignificantExamModeAddress();
  assert(*writingAddress != 0);
  size_t nbOfOnes = firstOneBit(*writingAddress, 32);
  // Compute the new value with two bits switched to 0.
  /* We write in a uint64_t instead of uint32_t, in case there was only one bit
   * left to 1 in writingAddress. */
  /* When writing in flash, we can only switch a 1 to a 0. If we want to switch
   * the fifth and sixth bit in a byte, we can thus write "11100111". */
  uint64_t deltaOnes = (1 << delta) - 1;
  uint64_t newValue = ~(deltaOnes << (32 + nbOfOnes - delta));
  // Write the value in flash
  /* Avoid writing out of sector */
  assert(writingAddress < (uint32_t *)&_exam_mode_buffer_end - 1 || *writingAddress > 1);
  size_t writtenFlash = *writingAddress == 1 ? sizeof(uint64_t) : sizeof(uint32_t);
  Ion::Device::Flash::WriteMemory((uint8_t *)writingAddress, (uint8_t *)&newValue, writtenFlash);
}

}
}
