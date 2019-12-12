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
 * activating or deactivating the exam mode we write one bit to 0. To determine
 * if we are in exam mode, we count the number of leading 0 bits. If it is even,
 * the exam mode is deactivated, if it is odd, the exam mode is activated. */

/* significantExamModeAddress returns the first uint32_t * in the exam mode
 * flash sector that does not point to 0. If this flash sector has only 0s, it
 * is erased (to 1) and significantExamModeAddress returns the start of the
 * sector. */

uint32_t * SignificantExamModeAddress() {
  uint32_t * persitence_start = (uint32_t *)&_exam_mode_buffer_start;
  uint32_t * persitence_end = (uint32_t *)&_exam_mode_buffer_end;
  while (persitence_start < persitence_end && *persitence_start == 0x0) {
    // Skip even number of zero bits
    persitence_start++;
  }
  if (persitence_start == persitence_end) {
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

bool FetchExamMode() {
  uint32_t * readingAddress = SignificantExamModeAddress();
  size_t numberOfLeading0 = 32 - firstOneBit(*readingAddress, 32);
  return numberOfLeading0 % 2 == 1;
}

void ToggleExamMode() {
  uint32_t * writingAddress = SignificantExamModeAddress();
  assert(*writingAddress != 0);
  // Compute the new value with one bit switched
  uint8_t numberOfLeadingZeroes = 32 - firstOneBit(*writingAddress, 32);
  /* When writing in flash, we can only switch a 1 to a 0. If we want to switch
   * the fifth bit in a byte, we can thus write "11110111". */
  uint32_t newValue = ~(1 << (31 - numberOfLeadingZeroes));

  // Write the value in flash
  Ion::Device::Flash::WriteMemory((uint8_t *)writingAddress, (uint8_t *)&newValue, sizeof(uint32_t));
}

}
}
